import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;

public class Main
{
    public static void main(String[] args) {
        int[][] arr = new int[37][49];

        String data = "";
        for(int[] row : arr)
        {
            for(int col : row )
            {
                data += (col + ", ");
            }
            data += "\n";
        }

        try {
           
            
            // Creates a FileOutputStream
            FileOutputStream fileOut = new FileOutputStream("Map.txt");
            
            // Creates an OutputStreamWriter
            OutputStreamWriter output = new OutputStreamWriter(fileOut);
            
            // Writes string to the file
            output.append(data);
            
            // Closes the writer
            output.close();
        }

        catch (Exception e) {
            e.getStackTrace();
        }
    }

    
}