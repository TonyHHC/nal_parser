# nal_parser 
這是 for ch12 的題目 
NAL : Neill’s Adventure Language

# nal 的 BNF 語法
## BNF 語法
```
<PROGRAM> := "{" <INSTRS>
<INSTRS> := "}" | <INSTRUCT> <INSTRS>
<INSTRUCT> := <FILE> | <ABORT> | <INPUT> | <IFCOND> | <INC> | <SET> |
<JUMP> | <PRINT> | <RND> 
```

+ Execute the instructions in file, then return here e.g. :  
```
FILE "test1.nal"  
<FILE> := "FILE" <STRCON>  
```
  
+ Halt/abort all execution right now !
```
<ABORT> := "ABORT" 
```
  
+ Fill a number variable with a number, or 2 string􀀀variables with string :
```
% IN2STR ( $C, $ZER ) or INNUM ( %NV )
<INPUT> := "IN2STR" "(" <STRVAR> "," <STRVAR> ")" | "INNUM" "(" <NUMVAR> ")" 
```
  
+ Jump to the nth word in this file (the first word is number zero!)
Brackets count as one word, "things in quotes" count as one word, e.g. :
```
JUMP 5
<JUMP> := "JUMP" <NUMCON> 
```
  
+ Output the value of variable, or constant, to screen with (without a linefeed)
```
<PRINT> := "PRINT" <VARCON>
<PRINTN> := "PRINTN" <VARCON> 
```
  
+ Set a variable to a random number in the range 0 to 99 e.g. :
```
RND ( %N )
Number should be seeded via the clock to be different on successive executions
<RND> := "RND" "(" <NUMVAR> ")" 
```
  
+ If condition/test is true, execute INSTRS after brace, else skip braces
```
<IFCOND> := <IFEQUAL> "{" <INSTRS> | <IFGREATER> "{" <INSTRS>
<IFEQUAL> := "IFEQUAL" "(" <VARCON> "," <VARCON> ")"
<IFGREATER> := "IFGREATER" "(" <VARCON> "," <VARCON> ")" 
```
  
+ Add 1 to a number variable e.g. :
```
INC ( %ABC )
<INC> := "INC" "(" <NUMVAR> ")" 
```
  
+ Set a variable. All variables are GLOBAL, and persist across the use of FILE etc.
```
$A = "Hello" or %B = 17.6
<SET> := <VAR> "=" <VARCON> | <VAR>
```
  
+ Some helpful variable/constant rules
```
% (Here ROT18 is ROT13 for letters and rot5 for digits)
<VARCON> := <VAR> | <CON>
<VAR> := <STRVAR> | <NUMVAR>
<CON> := <STRCON> | <NUMCON>
<STRVAR> := $[A-Z]+
<NUMVAR> := %[A-Z]+
<STRCON> := A plain text string in double quotes, e.g. "HELLO.TXT", or a ROT18 string in hashes e.g. #URYYB.GKG#
<NUMCON> := A number e.g. 14.301
```
