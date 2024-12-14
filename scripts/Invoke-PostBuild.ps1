
function Invoke-IsAdministrator  {  
    (New-Object Security.Principal.WindowsPrincipal ([Security.Principal.WindowsIdentity]::GetCurrent())).IsInRole([Security.Principal.WindowsBuiltinRole]::Administrator)  
}


function Get-ScriptDirectory {
    Split-Path -Parent $PSCommandPath
}



function Invoke-RunPostBuild {
    [CmdletBinding(SupportsShouldProcess)]
    param (
        [Parameter(Mandatory = $false, Position = 0)]
        [ValidateSet('Debug','Release')]
        [string]$Configuration="Debug",
        [Parameter(Mandatory = $false, Position = 1)]
        [ValidateSet('Win32','x64')]
        [string]$Platform="x64",
        [Parameter(Mandatory = $false, Position = 2)]
        [string]$RootPath = "$PSScriptRoot\..",
        [Parameter(Mandatory = $false, Position = 3)]
        [string]$ExeName = "AesCrypter.exe"
    )

      try{
        $IsAdministrator = Invoke-IsAdministrator 
        $ErrorDetails=''
        $ErrorOccured=$False
       
        if(!([string]::IsNullOrEmpty($Configuration))){
            $Configuration = $Configuration
            Write-Output "Configuration ==> $Configuration"
        }else{
            throw "missing argument 0"
        }
       
        if(!([string]::IsNullOrEmpty($Platform))){
            $Platform = $Platform
            Write-Output "Platform ==> $Platform"
        }else{
            throw "missing argument 1"
        }
        if(!([string]::IsNullOrEmpty($RootPath))){
            Write-Output "RootPath ==> $RootPath"
            $SolutionDirectory = (Resolve-Path $RootPath).Path
        }else{
            throw "missing argument 2"
        }
        
        $SolutionDirectory = (Resolve-Path $RootPath).Path
        $ScriptsDirectory = (Resolve-Path "$SolutionDirectory\scripts").Path
        $OutputDirectory = (Resolve-Path "$SolutionDirectory\bin\$Platform\$Configuration").Path

        $ExeFile = Get-ChildItem -Path "$OutputDirectory" -Filter "$ExeName" -Recurse -File
        if($ExeFile){
            Write-Output "Found! $($ExeFile.Filename)"
            $BuiltExecutable = $($ExeFile.Fullname)
        }else{
            $BuiltExecutable = Join-Path "$OutputDirectory" "$ExeName"
        }
        Write-Output "BuiltExecutable $BuiltExecutable"
        $ReadmeFile  = Join-Path "$SolutionDirectory" "usage.txt"
        Write-Output "=========================================================="
        Write-Output "SolutionDirectory ==> $SolutionDirectory"
        Write-Output "ScriptsDirectory  ==> $ScriptsDirectory"
        Write-Output "OutputDirectory   ==> $OutputDirectory"
        Write-Output "=========================================================="
        [string[]]$deps = . "$PSScriptRoot\dependencies\GetDependencies.ps1" -Path "$PSScriptRoot\dependencies"
        $depscount = $deps.Count
        $deps | % {
            . "$_"
        }
        $Test = Test-Dependencies -q
        if(! ($Test) ) { throw "dependencies error"} 

        Write-Output "`n`n"
        Write-Output "=========================================================="
        Write-Output "                POST-BUILD OPERATIONS"
        Write-Output "==========================================================`n`n"


        if([string]::IsNullOrEmpty($ENV:VersionPatcherPath)){
            Set-EnvironmentVariable -Name "VersionPatcherPath" -Value "$ENV:ProgramData\VersionPatcher\verpatch.exe" -Scope Session
            Set-EnvironmentVariable -Name "VersionPatcherPath" -Value "$ENV:ProgramData\VersionPatcher\verpatch.exe" -Scope User
            Write-Output "[warning] VersionPatcherPath is not setup in environment variables"
            if([string]::IsNullOrEmpty($ENV:VersionPatcherPath)){ throw "cannot configure verpathc path"}
        }

        $VersionPatcherPath = "$ENV:VersionPatcherPath"
        if(-not(Test-Path $VersionPatcherPath)){
            Copy-Item "$BuiltExecutable" "$ENV:VersionPatcherPath" -Force
        }

        $inf = Get-Item -Path "$BuiltExecutable"
        [DateTime]$CreatedOn = $inf.CreationTime
        $BuildDateString = $CreatedOn.GetDateTimeFormats()[13]
        [TimeSpan]$ts = [datetime]::Now - [DateTime]$CreatedOn
        $Md5Hash = (Get-FileHash "$BuiltExecutable" -Algorithm MD5).Hash
        $NewInternalName = "{0}-{1}" -f $inf.Basename, $Configuration
        
        $Log = 'Built {0:d2} hours, {1:d2} minutes and {2:d2} seconds ago' -f $ts.Hours, $ts.Minutes, $ts.Seconds
        $NewDescription = "A command-line utility that reads and write, updates the version information in a Windows binary file."
        $BuildComment = '{2} build created on {0} using the pc {1}. MD5: {3}' -f $BuildDateString, "$ENV:COMPUTERNAME", $Configuration, $Md5Hash
        
        $VerPatchCompatible = Test-VerPatchCompatible "$BuiltExecutable"
        if($VerPatchCompatible -eq $False){
            Write-Output "[IMPORTANT] NEW BUILD `"$BuiltExecutable`" does not have binary embedded version information. Initializing version info..."
            $LogString = @"
Setting Version Values
    VersionSring $VersionSring
    Description  $NewDescription
    ProductName  $NewInternalName
    SpecialBuild True
    RESSOURCE    `"$ReadmeFile`"
"@
            Write-Output "$LogString"
            $VersionSring = "1.0.0.1"
            &"$ENV:VersionPatcherPath" "$BuiltExecutable" '/va' "$VersionSring" '/s' 'desc' "`"$NewDescription`""   `
                '/s' 'ProductName' "$NewInternalName" `
                '/s' 'PrivateBuild' "$Md5Hash" `
                '/rf' '#64' "$ReadmeFile" `
                '/pv' "$VersionSring" 

           $VerPatchCompatible = Test-VerPatchCompatible "$BuiltExecutable"

           [string[]]$VersionData = &"$ENV:VersionPatcherPath" "$BuiltExecutable"
            $LogString = @"
   === REPORT ===
File `"$BuiltExecutable`"
Enabled VerPatchCompatibility $VerPatchCompatible
Version Data 

"@
            $VersionData | % {
                $LogString += "`t$_`n"
            }
           Write-Output "$LogString"
           if(!($VerPatchCompatible)){
                throw "Error when updating version"
           }
        }else{
            [string[]]$VersionData = &"$VersionPatcherPath" "$BuiltExecutable"
            [Version]$CurrentVersion = Get-VerPatchProperty "$BuiltExecutable" 'Version'
            Write-Host "$CurrentVersion" -f Red
            $NewBuildVersion = $CurrentVersion.Build + 1
            $NewVersion = [version]::new($CurrentVersion.Major, $CurrentVersion.Minor, $NewBuildVersion, $CurrentVersion.Revision)
            [string]$VersionString = $NewVersion.ToString()

            $Strlog =@"
`"`$ENV:VersionPatcherPath`" `"`$BuiltExecutable`" `"`$VersionString`" '/s' 'desc' `"``"`$NewDescription``"`" '/s' 'ProductName' `"`$NewInternalName`" `
'/s' 'PrivateBuild' `"`$Md5Hash`" `
'/rf' '#64' `"`$ReadmeFile`" `
'/pv' `"`$VersionSring`" 

`"$ENV:VersionPatcherPath`" `"$BuiltExecutable`" `"$VersionString`" '/s' 'desc' `"``"$NewDescription``"`"   `
                '/s' 'ProductName' `"$NewInternalName`" `
                '/s' 'PrivateBuild' `"$Md5Hash`" `
                '/rf' '#64' `"$ReadmeFile`" `
                '/pv' `"$VersionSring`" 

"@
            Write-Host "$Strlog" -f DarkRed
            &"$ENV:VersionPatcherPath" "$BuiltExecutable" "$VersionString" '/s' 'desc' "`"$NewDescription`""   `
                '/s' 'ProductName' "$NewInternalName" `
                '/s' 'PrivateBuild' "$Md5Hash" `
                '/rf' '#64' "$ReadmeFile" `
                '/pv' "$VersionSring" 
        }
       


        Write-Output "=========================================================="
        Write-Output "        POST-BUILD OPERATIONS COMPLETED SUCCESFULLY       "
        Write-Output "==========================================================`n`n"


    }catch{
        Show-ExceptionDetails ($_) -ShowStack
        $ErrorDetails= "$_"
        $ErrorOccured=$True
    }
    if($ErrorOccured){
        Start-Sleep 2
        throw "$ErrorDetails"
    }

}


Invoke-RunPostBuild 