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


