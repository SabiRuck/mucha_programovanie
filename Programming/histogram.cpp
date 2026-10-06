#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

char direction;//v alebo h
int n; //kolko cisel zadam
int m; //ktorym cislom zacina rozsah v ktorom mam zadat tie cisla - m+8 koniec
vector<int> numInp;//zoznam zadanych cisel uzivatelom
int repetition[9] = {0};//pocet "#" pre stlpce
int incNum = 0;//pocet zlych cisel

int main()
{
    //tieto  nacitavaju data
    cin >> direction;
    if(direction != 'h' && direction != 'v') {cout << "Neplatný mód vykreslenia"<< endl; return 1;}

    cin >> n >> m;

    for (int i = 0; i < n; i++)
    {
        int cislo;
        cin >> cislo;             
        numInp.push_back(cislo);
    }
        
    //toto pocita ten ihstogram
    for(int i=0; i<n; i++)
    {
        int x = numInp[i]-m;
        if(x>8 || x<0) incNum++;
        else repetition[x]++;
    }

    //vypisy
    if(direction =='h')
    {
        bool sameNumLeng = (to_string(m).length() == to_string(m+8).length()) ? true : false;


        for(int i=0; i<9; i++)
        {
            if(sameNumLeng || (to_string(m).length() < to_string(m+i).length()))
            {
                cout << m+i << ": ";
            }
            else
            {
                cout << "_" << m+i << ": ";
            }
            
            cout << string(repetition[i], '#') << endl;
        }
        if(incNum>0) cout << "invalid: " << string(incNum, '#') << endl;

    }
    else
    {
        int x = *max_element(repetition, repetition + 9);
        if(x<incNum) x = incNum;

        for(x; x>0; x--)
        {
            cout << ((x<=incNum) ? "#" : " ");

            for(int i=0; i<9; i++)
            {
                cout << ((x<=repetition[i]) ? "#" : " ");
            }

            cout << endl;
        }

        cout << 'i';
        for(int i=0; i<9; i++)
        {
            cout << m+i;
        }
        cout << endl;



    }





    
    
    return 0;
}