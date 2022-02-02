#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<stdlib.h>
using namespace std;
#define Char_size 256                                                                                  
struct Node                                                                                            
{
    unsigned char character;
    long long int Freq;
    Node* left;
    Node* right;
    Node(unsigned char c,long long int f,Node* l=NULL,Node* r=NULL)
    {
        character=c;
        Freq=f;
        left=l;
        right=r;
    }
};
void Mindownheap(vector<Node*> &A,int i,int length)                                                     
{
    int least=i;
    if(2*i+1<=length&&A[2*i+1]->Freq<A[i]->Freq)
    {
        least=2*i+1;
        if(2*i+2<=length&&A[2*i+2]->Freq<A[2*i+1]->Freq)
            least=2*i+2;
    }
    else if(2*i+2<=length&&A[2*i+2]->Freq<A[i]->Freq)
        least=2*i+2;
    if(least!=i)
    {
        swap(A[i],A[least]);
        Mindownheap(A,least,length);
    }
}
Node* Extract_min(vector<Node*> &A)                                                                  
{
    if(A.size()<1)
        return NULL;
    Node* minimum=A[0];
    A[0]=A.back();
    A.pop_back();
    Mindownheap(A,0,A.size()-1);
    return minimum;

}
void Insert_Min(vector<Node*> &A,Node* element)                                                  //Insert Character in Min
{
    A.push_back(element);
    int i=A.size()-1;
    while(i>0&&A[(i-1)/2]->Freq>A[i]->Freq)
    {
        swap(A[i],A[(i-1)/2]);
        i=(i-1)/2;
    }
}
void Build_Min(vector<Node*> &A,int length)
{
    for(int i=(length-1)/2;i>=0;i--)
    {
        Mindownheap(A,i,length);
    }
}
void store_codes(Node* Root,char single_code[],int index,vector<long long int> &Huffman_codemap)    //Store code for each character in Vector
{
    if(Root->left)
    {
        single_code[index]='0';
        store_codes(Root->left,single_code,index+1,Huffman_codemap);

    }
    if(Root->right)
    {
        single_code[index]='1';
        store_codes(Root->right,single_code,index+1,Huffman_codemap);
    }
    if(!Root->left&&!Root->left)
    {
        for(int i=index;i>=0;i--)
        {
          if(i!=index)
          {
            Huffman_codemap[Root->character]*=10;
            Huffman_codemap[Root->character]+=single_code[i]-'0';
          }
          else
            Huffman_codemap[Root->character]=1;
        }
    }
}
void store_tree(ofstream &input,Node* Root)                                                          //Write tree to file
{
    if(!Root->left&&!Root->right)
    {
        input<<'1';
        input<<Root->character;
    }
    else
    {
        input<<'0';
        store_tree(input,Root->left);
        store_tree(input,Root->right);
    }
}
Node* Huffman(long long int Count[])                                                                //Main Huffman Algorithm
{
    vector<Node*>min;
    for(int i=0;i<Char_size;i++)
        if(Count[i]!=0)
            min.push_back(new Node(i,Count[i]));
    Build_Min(min,min.size()-1);
    while(min.size()!=1)
    {
        Node* Z=new Node(-1,0,Extract_min(min),Extract_min(min));
        Z->Freq=Z->left->Freq+Z->right->Freq;
        Insert_Min(min,Z);
    }
    return(min[0]);
}
void Write_compressed(ifstream &input,ofstream &output,vector<long long int > &Huffman_codemap)    //Write to a new file(Compressed)
{
    char ch;
    unsigned char bits_8;
    long long int counter=0;
    while(input.get(ch))
    {
        long long int temp=Huffman_codemap[static_cast<unsigned char>(ch)];
        while(temp!=1)
        {
          bits_8<<=1;
          if((temp%10)!=0)
                bits_8|=1;
          temp/=10;
          counter++;
          if(counter==8)
            {
                output<<bits_8;
                counter=bits_8=0;
            }
        }
    }
    while(counter!=8)
    {
        bits_8<<=1;
        counter++;
    }
    output<<bits_8;
    output.close();
}
int main(int argc,char *argv[])
{
    int conti=1;
    do{
    vector<long long int > Huffman_codemap;                                                         //Double dimensional vector to store Huffman codes
    Huffman_codemap.resize(Char_size);
    long long int Count[Char_size]={0};                                                             //Declare and initialize character count array
    string filename;                                                                                //Set filename
    cout<<"Enter Filename:\t";
    cin>>filename;
    ifstream input_file(filename.c_str(),ios::binary);                                              //set input stream
    if(!input_file.good())                                                                          //Check if input file exists
    {
        perror("Error:\t");
        exit(-1);
    }
    cout<<"\nCompressing the file.....";
    char ch;
    while(input_file.get(ch))                                                                       //Read file and increase count
        Count[static_cast<unsigned char>(ch)]++;
    input_file.clear();                                                                             //Reset File pointers
    input_file.seekg(0);                                                                            //Reset File pointers
    Node* tree=Huffman(Count);                                                                      //create Huffman tree
    ofstream output_file((filename+".huf").c_str(),ios::binary);                                    //Write file name
    if(!output_file.good())                                                                         //Check if file can be written
    {
        perror("Error:\t");
        exit(-1);
    }
    output_file<<tree->Freq;                                                                        //Write to file
    output_file<<',';
    store_tree(output_file,tree);
    output_file<<' ';
    char single_code[16];                                                                           //array to store single code
    store_codes(tree,single_code,0,Huffman_codemap);                                                //Store Codes in vector
    Write_compressed(input_file,output_file,Huffman_codemap);                                       //Write to file
    input_file.close();                                                                             //Close file stream
    output_file.close();                                                                            //Close file stream
    cout<<"\n\nFile Compressed Successfully!\n\n";
    cout<<"enter 1 to continue or 0 to exit..";
    cin>>conti;
    }while(conti!=0);
    return 0;
}