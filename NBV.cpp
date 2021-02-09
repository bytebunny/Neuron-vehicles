#include"NBV.h"

/// Write out the logs for all the vehicles to a matlab/octave file
/// Candidate for a friend function to binoculus, writelog(name,var,lognum)
void writevehiclelogs( std::string fname, char var, int lnum )
{
    std::list<Binoculus>::iterator iter;

    std::ofstream sfid(fname.c_str(), ios::out | ios::binary);

    if(GS.logging){
        if (sfid.fail()){
            cout<<"Could not save file "+fname<<endl;
        }else{
            for (iter=globalBvl.begin(); iter != globalBvl.end(); iter++) {
                //-char buffer2[100];
                //-sprintf(buffer2,"V%c%d",var,(*iter).getid());
                std::ostringstream Convert;
                Convert << "V"<<var<<(*iter).getid();
                std::string buffer2= Convert.str();
                //cout << buffer2<<endl;
                if (lnum==2)
                    (*iter).log2.writematlabv4(sfid,buffer2);
                else
                    (*iter).log.writematlabv4(sfid,buffer2);
                cout <<"Writing log "<<lnum<<" for "<<(*iter).name <<endl;
            }
            sfid.close();
        }
    }else{// not logging
        cout << "Please set logging on in bv.ini"<<endl;
    }
} // end writevehiclelogs
