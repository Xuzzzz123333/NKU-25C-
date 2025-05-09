# 设置控制台输出编码为UTF-8
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8

# 获取当前目录下所有.h和.cpp文件
$files = Get-ChildItem -Path . -Filter *.h -Recurse
$files += Get-ChildItem -Path . -Filter *.cpp -Recurse

# 遍历所有文件
foreach ($file in $files) {
    Write-Host "Processing file: $($file.FullName)"
    
    # 读取文件内容
    $content = Get-Content -Path $file.FullName -Raw -Encoding Default
    
    # 使用UTF-8编码(不带BOM)重新写入文件
    $utf8NoBomEncoding = New-Object System.Text.UTF8Encoding $false
    [System.IO.File]::WriteAllText($file.FullName, $content, $utf8NoBomEncoding)
    
    Write-Host "Converted $($file.FullName) to UTF-8 without BOM"
}

Write-Host "All files have been converted!" 