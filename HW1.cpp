#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <fstream>

using namespace std;
auto compare = [](const pair<string, int> &l, const pair<string, int> &r)
{
    //Determine if the character is EOL
    if(l.second == r.second)
    {
        if(l.first == "<EOL>")
        {
            return false;
        }
        else if(r.first == "<EOL>")
        {
            return true;
        }
        else
        {
            
        return l.first > r.first;
        }
    }
    else
    {
        return l.second < r.second;
    }
};

unordered_map<string,int> getCount(vector<string> input)
{
    //Take the vector holding the input characters
    unordered_map<string,int> freq;
    for(auto &x:input)

    {
        //Store each charater from the buffer into a map getting the frequency as well
        freq[x]++;
    }
    return freq;

}
//Convert a map of <String, int> into a vector

vector<pair<string,int>> convertVector(unordered_map<string,int> freq)
{

    vector<pair<string,int>> myVector;
    //Insert elements into the vector
    std::copy(freq.begin(),freq.end(),back_inserter(myVector));
    return myVector;
}
void sortVector(vector<pair<string,int>> &myV)
{
    //Sort the vector based on ascii value from comparsion function
    sort(myV.begin(),myV.end(),compare);
    reverse(myV.begin(), myV.end());
}
void displayFreq(vector<pair<string,int>> myV)
{
    for(auto &x: myV)
    {
        //Print the frequenct of the characters
        cout << x.first << " frequency" << " = " << x.second << endl;
    }
}

int main()
{
    
    vector<string> list;
    string input;
    getline(cin,input,'\0');
    //Read input by re-direction 
    for(int i = 0; i < input.size(); i++)
    {

        if(input[i] == '\n')
        {
            //Store into vector each character if it is a newline store <EOL> instead
            list.push_back("<EOL>");

        }
        else
        {
		string letter(1,input[i]);
            list.push_back(letter);
        }


    }

    // Calling functions and storing them in appropiate variables

    unordered_map<string,int> freqs = getCount(list);
    vector<pair<string,int>> primary = convertVector(freqs);
    sortVector(primary);
    displayFreq(primary);
    //Creating pid
    pid_t pid;
    //getting size of queue to use in loop
    int n = primary.size();

   vector<pair<string,int>> filenames;
   //Initiate filenames to primary since each character will have its own file and primary contains all the characters from input file

   filenames = primary;
   cout << "Original Message:\t";
   for(auto &x: list)
   {
       //displaying the original message before compression
       cout << x;
   }
   cout << endl;
   for(int i = 0; i < n; i++)
   {
       //get first element in queue
       pair<string,int> x = primary.front();
       
       string bitstring = "";
       vector<string> remMessage = list;
       pid = fork();
       if(pid == 0) // children working
       {
           if(!primary.empty())
           {
               for(int j = 0; j < list.size(); j++)
               {
                   //Compute bitstring if the current chacater is matching then apend 1 to the bitring else append 0
                   if(x.first == list.at(j))
                   {

                       bitstring += "1";
                   }
                   else
                   {
                       bitstring += "0";
                   }

               }
           }
            //Create the ofstream object for each character
           ofstream outputFile;
           string filename = x.first;
           //name the file by the character + .txt extension
           filename.append(".txt");
           outputFile.open(filename);
           
           if(x.first != "<EOL>")
           {
           outputFile << "Symbol " << x.first << " code:\t" <<'\t' <<  bitstring << endl;
           }
           else
           {
           outputFile << "Symbol " << x.first << " code:\t"  <<  bitstring << endl;
           }
           if(primary.size() > 1)
           {
           outputFile << "Remaining Message:\t";
           //Remove the character from the message and grab next character from queu sinc ethe loop will start again 
            list.erase(std::remove(list.begin(),list.end(), x.first), list.end());
            // Update remaning message after removing character
            remMessage = list;

           for(auto &x:remMessage)
           {
               outputFile << x;
           }
           }
           outputFile.close();

            //exit child 
           _exit(0);

       }
        list.erase(std::remove(list.begin(),list.end(), x.first), list.end()); // Parent passes child proccess information about message and frequency
       primary.erase(primary.begin());

   }
   //END loop
   
   for(int i =0; i < n; i++)
   {
       //parent waiting for children to end;
       wait(0);
   }
    // Parent generating compression message
    int printSize = 1;
    for(auto &file:filenames)
    {
        
        ifstream fileRead;
        string myfile = file.first;
        string message;
        myfile.append(".txt");
        fileRead.open(myfile);
        getline(fileRead,message,'\0');
        if(printSize != filenames.size())
        {
        cout << message << endl;
        }
        else
        {
            cout << message;
        }
        printSize++;
        fileRead.close();
    
    }


    

    return 0;
}
