#!/bin/csh
set user = $1
set Expected = HelloFrom.ygong60.RNyKrvLfNgOqfINzlBXNmktbeBZplbyasssEQkVoSu
set Actual = `./BreakRSA  5306816277667613 2819744428798523  2915932169341812 3379476769862880 2968394936259487 460043752902980 1276170043804177 199291053030758 1440569895213769 4860535207486440 4552820095841344 3055911033428714`
echo "expected is $Expected" 
echo "actal    is $Actual"
if ( "$Expected" == "$Actual" ) then
echo "Grade for user $user is 100"
else
echo "Grade for user $user is 50"
endif
