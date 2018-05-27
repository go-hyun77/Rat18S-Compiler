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
     
"SyntaxAnalyzer.h" contains syntax rules (rewritten to remove any possible left recursion generated in parse trees) that are outputted in "output.txt" simultaneously as the lexer() function parses and generates tokens. The appropriate production rules used are outputted under the generated token and lexeme. If syntax errors exist, the program will output an error message as well as the line number the syntax error is detected on.

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

      Token: Operator       Lexeme:   :                   
            <Empty> -> Epsilon

      Token: Keyword        Lexeme:   int                 
            <Qualifier> ->  Int | Boolean | Real
            <Qualifier> -> Int
            
      ....
      ...
      ..
      
