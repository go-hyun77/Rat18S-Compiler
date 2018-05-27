## Disclaimer
All credit for fictional production rules and Rat18S semantics belong to Professor James Song Choi of California State University, Fullerton.


Rat18S is a fictional language created for the purpose of demonstrating lexical analysis, syntax analysis, and intermediate code generation. A simple test case is as follows.


      function convert$ [fahr:int]
      {
                 return 5*(3 - 32) / 9;
      }

      %%
           int    low,  high, step$;      ! declarations !

           get (low, high, step$);
           while (low  <  high )  
               {  put (low);
                   put (convert$ (low));
                   low =  low  +  step$;
               } 




"LexicalAnalyzer.h" contains lexer(), which parses a text file containing sample code of Rat18S. Tokens generated are returned to a vector, which is then outputted in the text file "lexeme.txt" as follows.

      token          lexeme

      function       Keyword
      convert$       Identifier
      [              Seperator
      fahr           Identifier
      :              Operator
      int            Keyword
      ]              Seperator
      .....
      ....
      ...
      ..
     
     

"SyntaxAnalyzer.h" contains production rules rewritten to remove any possible left recursion generated in subsequent parse trees. The appropriate production rules used are outputted under the generated token and lexeme simultaneously as lexer() executes. If syntax errors exist, the program will output an error message as well as the line number the syntax error is detected on. All production rules can be viewed in "ProductionRules.txt".

      Token: Keyword        Lexeme:   function            
	<Rat18S> -> <Opt Function Definitions> %% <Opt Declaration List> <Statement List> 
	<Opt Function Definitions> ->  <Function Definitions> | <Empty>
	<Function Definitions> -> <Function> | <Function> <Function Definitions>
	<Function> ->  function <Identifier> [ <Opt Parameter List> ] <Opt Declaration List> <Body>

      Token: Identifier     Lexeme:   convert$            

      Token: Seperator      Lexeme:   [                   

      Token: Identifier     Lexeme:   fahr                
            <Opt Parameter List> -> <Parameter List> | <Empty>	<Parameter List> -> <Parameter> | <Parameter>, <Parameter List>
            <Parameter> -> <IDs> : <Qualifier> 
            <IDs> -> <Identifier> | <Identifier>, <IDs>
            
      ....
      ...
      ..
     
     
     
Each "identifier" parsed by lexer() will be stored in an array symbolTable[]. Fictional assembly code instructions generated will be stored in an array instrTable[] and outputted on the console upon execution of the program. A variable "memory_address" holds the initial value of 2000.


	__Symbol__     __Type__       __Memory Address__
	fahr           int            2000
	low            int            2001
	high           int            2002
	step$          int            2003

	__Index__      __Operation__  __Operand__
	1              PUSHI          5
	2              PUSHI          3
	3              PUSHI          32
	4              SUB            
	5              MUL            
	6              PUSHI          9
	7              DIV            
	8              STDIN          
	9              POPM           2001
	10             STDIN          
	11             POPM           2002
	12             STDIN          
	...
	..
	
	
