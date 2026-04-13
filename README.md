<img width="593" height="593" alt="a-simple-flat-design-blue-butterfly-icon-perfect-f-removebg-preview" src="https://github.com/user-attachments/assets/cd817672-cf70-4574-9833-46c1cdaedcc3" />

# Cycles Programming Language

Cycles is a programming language that mixes C# and GDScript together

```
import operatingio;

extends OSInteractionTest
{
    void Main()
    {
        string filename = "logs.txt";

        println("Initializing operatingio...");
        
        operatingio.createFile("test_output.txt");

        println("Checking directory structure:");
        operatingio.runProcess("dir");

        println("Process Complete.");
        hold;
    }
}
```
