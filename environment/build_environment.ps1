<#
.SYNOPSIS
    This script is used to create a conda environment for the infrastructure testing.

.DESCRIPTION
    The order of the arguments is important with the first argument being the action to be taken
    and the second argument being the path to the conda environment though the path is optional.

.PARAMETER Action
    The action to be taken: -h (help), -c (clean), -s (setup), -su (setup-update).

.PARAMETER Path
    The path to the conda environment.

.EXAMPLE
    .\build_environment.ps1 -Action -s -Path "C:\path\to\env"
#>

param (
    [Parameter(Mandatory=$true)]
    [ValidateSet("-h", "--help", "-c", "--clean", "-s", "--setup", "-su", "--setup-update")]
    [string]$Action,

    [Parameter(Mandatory=$false)]
    [string]$Path
)

function Show-Help {
    Write-Output "This script is used to create a conda environment for the infrastructure testing"
    Write-Output ""
    Write-Output "The order of the arguments is important with the first argument being the action to be taken"
    Write-Output "and the second argument being the path to the conda environment though the path is optional"
    Write-Output "Syntax: build_environment.ps1 -Action [-h|c|s|su] -Path <path>"
    Write-Output "options:"
    Write-Output "-h  | --help             Print this Help."
    Write-Output "-c  | --clean            Clean the conda environment."
    Write-Output "-s  | --setup            Setup the conda environment."
    Write-Output "-su | --setup-update     Setup the conda environment and update it."
    Write-Output ""
    Write-Output "-Path      The path to the conda environment."
}

function Find-InCondaEnv {
    param (
        [string]$EnvName
    )
    # We need to insert extra backslashes to escape the backslash in the path to make it a valid pattern (idk why)
    $EnvName = $EnvName -replace '\\', '\\'
    $envList = conda env list
    # Determine if the path is envionment is already in the environment list
    return (Select-String -InputObject $envList -Pattern $EnvName -Quiet)
    # if (Select-String -InputObject $envList -Pattern $EnvName -Quiet) {
    #     return $true
    # } else {
    #     return $false
    # }

}

function Create-CondaEnv {
    param (
        [string]$EnvPath
    )
    if (-not $EnvPath) {
        conda env create --name world-generation --file=$Env:PROJECT_ROOT\environment\environment.yaml
    } else {
        conda env create --prefix $EnvPath --file=$Env:PROJECT_ROOT\environment\environment.yaml
    }
}

function Activate-CondaEnv {
    param (
        [string]$EnvPath
    )
    if (-not $EnvPath) {
        conda activate world-generation
    } else {
        conda activate $EnvPath
    }
}

function Update-CondaEnv {
    param (
        [string]$EnvPath
    )
    if (-not $EnvPath) {
        conda env update --name world-generation --file=$Env:PROJECT_ROOT\environment\environment.yaml
    } else {
        conda env update --prefix $EnvPath --file=$Env:PROJECT_ROOT\environment\environment.yaml
    }
}

function Deactivate-CondaEnv {
    conda deactivate
}

function Remove-CondaEnv {
    param (
        [string]$EnvPath
    )
    if (-not $EnvPath) {
        conda env remove -n world-generation -y
    } else {
        conda env remove -p $EnvPath -y
    }
}

switch ($Action) {
    "-h" { Show-Help }
    "--help" { Show-Help }
    "-c" { 
        Write-Output "Removing the conda environment"
        Deactivate-CondaEnv
        if (-not $Path) {
            Write-Output "No conda environment path has been passed in will use the default path"
            Remove-CondaEnv
        } else {
            Remove-CondaEnv -EnvPath $Path
        }
    }
    "--clean" { 
        Write-Output "Removing the conda environment"
        Deactivate-CondaEnv
        if (-not $Path) {
            Write-Output "No conda environment path has been passed in will use the default path"
            Remove-CondaEnv
        } else {
            Remove-CondaEnv -EnvPath $Path
        }
    }
    "-su" { 
        if (-not $Path) {
            Write-Output "No conda environment path has been passed in will use the default path"
            $result = Find-InCondaEnv -EnvName "world-generation"
            if ($result -eq $true) {
                Write-Output "The conda environment exists already - updating the environment"
                Update-CondaEnv
            } else {
                Write-Output "Creating the conda environment"
                Create-CondaEnv
            }
            Write-Output "Activating the conda environment"
            Activate-CondaEnv
        } else {
            Write-Output "The conda environment path is $Path"
            $result = Find-InCondaEnv -EnvName $Path
            if ($result -eq $true) {
                Write-Output "The conda environment exists already - updating the environment"
                Update-CondaEnv -EnvPath $Path
            } else {
                Write-Output "Creating the conda environment"
                Create-CondaEnv -EnvPath $Path
            }
            Write-Output "Activating the conda environment"
            Activate-CondaEnv -EnvPath $Path
        }
    }
    "--setup-update" { 
        if (-not $Path) {
            Write-Output "No conda environment path has been passed in will use the default path"
            $result = Find-InCondaEnv -EnvName "world-generation"
            if ($result -eq $true) {
                Write-Output "The conda environment exists already - updating the environment"
                Update-CondaEnv
            } else {
                Write-Output "Creating the conda environment"
                Create-CondaEnv
            }
            Write-Output "Activating the conda environment"
            Activate-CondaEnv
        } else {
            Write-Output "The conda environment path is $Path"
            $result = Find-InCondaEnv -EnvName $Path
            if ($result -eq $true) {
                Write-Output "The conda environment exists already - updating the environment"
                Update-CondaEnv -EnvPath $Path
            } else {
                Write-Output "Creating the conda environment"
                Create-CondaEnv -EnvPath $Path
            }
            Write-Output "Activating the conda environment"
            Activate-CondaEnv -EnvPath $Path
        }
    }
    "-s" { 
        if (-not $Path) {
            Write-Output "No conda environment path has been passed in will use the default path"
            $result = Find-InCondaEnv -EnvName "world-generation"
            if ($result -eq $true) {
                Write-Output "The conda environment exists already"
            } else {
                Write-Output "Creating the conda environment"
                Create-CondaEnv
            }
            Write-Output "Activating the conda environment"
            Activate-CondaEnv
        } else {
            Write-Output "The conda environment path is $Path"
            $result = Find-InCondaEnv -EnvName $Path
            if ($result -eq $true) {
                Write-Output "The conda environment exists already"
            } else {
                Write-Output "Creating the conda environment"
                Create-CondaEnv -EnvPath $Path
            }
            Write-Output "Activating the conda environment"
            Activate-CondaEnv -EnvPath $Path
        }
    }
    "--setup" { 
        if (-not $Path) {
            Write-Output "No conda environment path has been passed in will use the default path"
            $result = Find-InCondaEnv -EnvName "world-generation"
            if ($result -eq $true) {
                Write-Output "The conda environment exists already"
            } else {
                Write-Output "Creating the conda environment"
                Create-CondaEnv
            }
            Write-Output "Activating the conda environment"
            Activate-CondaEnv
        } else {
            $result = Find-InCondaEnv -EnvName $Path
            if ($result -eq $true) {
                Write-Output "The conda environment exists already"
            } else {
                Write-Output "Creating the conda environment"
                Create-CondaEnv -EnvPath $Path
            }
            Write-Output "Activating the conda environment"
            Activate-CondaEnv -EnvPath $Path
        }
    }
    default { Write-Output "Invalid option: $Action" }
}