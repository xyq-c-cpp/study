module main

go 1.16

require cli/mycli v1.0.0
require demo v1.0.0

//replace 需要的包 本地需要的包的路径
replace cli/mycli => D:/code/go/src/cli
replace demo => D:/code/go/src/demo