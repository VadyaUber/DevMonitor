$Inputstring = "A a B b C c D d E e F f G g H h I i J j K k L l M m N n O o P p Q q R r S s T t U u V v W w X x Y y Z z"
$WeekArray = $InputString.Split(" ")
$WeekNum = get-date -UFormat %V;
$YearNum = (get-date -UFormat %y) % 10;

$Weekcode = $WeekArray[$WeekNum-1]
$Version = "$Weekcode$YearNum"

$defstr="#define VERSION_BUILD ";

$string_vers = “$defstr“"$Version”"”
#Write-host $string_vers 
#Write-host (Get-Content ./version.h)[1]

if ($string_vers -eq (Get-Content ./version.h)[1])
{
Write-host "Current version already writed"
exit 0
}
else
{
(Get-Content -path ./version.h -Raw) |
    ForEach-Object {
        
        $regex="$defstr(?<BuildVersion>.*)";
        if($_ -match $regex) {
            if ($_ -like "*$Version*") { 
            Write-host "Current version already writed"
            }
            else{
            Write-host "Version updated"
            $_ = $_ -replace $regex,“$defstr“"$Version”"`r”
            }
        }
        $_
    } |
    Out-File ./version.h -encoding ascii -nonewline

exit 0
}

<#
enum WeekCoded {
    A = 1;
    a = 2;
    B = 3;
    b = 4;
    C = 5;
    c = 6;
    D = 7;
    d = 8;
    E = 9;
    e = 10;
    F = 11;
    f = 12;
    G = 13;
    g = 14;
    H = 15;
    h = 16;
    I = 17;
    i = 18;
    J = 19;
    j = 20;
    K = 21;
    k = 22;
    L = 23;
    l = 24;
    M = 25;
    m = 26;
    N = 27;
    n = 28;
    O = 29;
    o = 30;
    P = 31;
    p = 32;
    Q = 33;
    q = 34;
    R = 35;
    r = 36;
    S = 37;
    s = 38;
    T = 39;
    t = 40;
    U = 41;
    u = 42;
    V = 43;
    v = 44;
    W = 45;
    w = 46;
    X = 47;
    x = 48;
    Y = 49;
    y = 50;
    Z = 51;
    z = 52;
}
#>