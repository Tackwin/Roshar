$files = New-Object System.Collections.Generic.List[System.Object]

Get-ChildItem -Recurse -Path . -File | ForEach-Object {
	if ($_.Extension -eq ".cpp" -or $_.Extension -eq ".hpp") {
		files.Add($_.FullName)
	}
}