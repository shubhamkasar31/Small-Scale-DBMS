#include <bits/stdc++.h>
#include <fstream>
using namespace std;

fstream schema;
fstream table;

bool isTablePresent(string tablename){
    schema.open("schema.txt",ios::in);
    string line;
    while(getline(schema,line)){
        istringstream s(line);
        string lclTable;
        getline(s,lclTable,'#');
        if(lclTable == tablename){
            schema.close();
            return true;
        }
    }
    schema.close();
    return false;
}


int colCalculator(string tablename){
    table.open(tablename+".txt",ios::in);
    string line;
    getline(table,line);
    istringstream s(line);
    string t;
    int cnt=0;
    while(getline(s,t,'#')){
        cnt++;
    }
    table.close();
    return cnt;
}

bool validate_att_name(string s){
    int sl = s.length();
    if(sl>256)
        return false;
    if(!isalpha(s[0]))  return false;
    for(int t=0;t<sl;t++){
        if(!isalnum(s[t]) && s[t]!='_')
            return false;
    }
    return true;
}

bool validate_att_typ(string s){
    int sl=s.length();
    if(s == "int"){
        return true;
    }
    bool ret;
    string sub_s = s.substr(0,8);
    if(sub_s == "varchar(")
        ret = true;
    else return false;
    if(s[sl-1] == ')')
        ret = true;
    else ret = false;
    if(ret == false) return false;

    string num = s.substr(8,sl-9);
    for(int t=0;t<num.length();t++)
        if(!isdigit(num[t]))
            return false;
    return true;
}

vector<string> att_seq(string tablename){
    schema.open("schema.txt",ios::in);
    vector<string> ret;
    string rec;
    while(getline(schema,rec)){
        istringstream s(rec);
        string lclTable;
        getline(s,lclTable,'#');
        if(lclTable==tablename){
            break;
        }
    }
    schema.close();
    string rec1=rec;
    //cout<<"rec: "<<rec<<endl;
    istringstream st(rec1);
    string temp,attri;
    getline(st,temp,'#');
    int cnt=0;
    while(getline(st,temp,'#')){
        if(cnt==1){
            ret.push_back(temp);
            cnt=0;
        }else cnt=1;
    }
    /*for(int i=0;i<ret.size();i++){
        cout<<ret[i]<<" ";
    }
    cout<<endl;*/
    return ret;
}


bool valid(string st,vector<string> att_sq){
    vector<string> sq;
    for(int i=0;i<att_sq.size();i++){
        if(att_sq[i]!="int"){
            att_sq[i]=att_sq[i].substr(0,7);
        }
    }
    string tmp;
    istringstream s(st);
    while(getline(s,tmp,',')) sq.push_back(tmp);

    //for(int i=0;i<att_sq.size();i++)cout<<att_sq[i]<<" ";
    //cout<<endl;
    //cout<<endl;
    //cout<<"ATT_SQ: "<<att_sq.size()<<" SQ: "<<sq.size()<<endl;

    for(int i=0;i<att_sq.size();i++){
        bool flag=true;
        for(int j=0;j<sq[i].length();j++){
            if(sq[i][j]>='0' && sq[i][j]<='9') flag=true;
            else {flag=false;break;}
        }
        if(att_sq[i]=="int" && flag==false) return false;
        else if(att_sq[i] == "varchar" && flag) return false;
    }
    return true;
}



void query_processing(){
    string query;
    cout<<"\nEnter the query: ";
    cin.ignore();
    getline(cin,query);
    transform(query.begin(), query.end(), query.begin(), ::tolower);
    string operation;
    stringstream s(query);

    s>>operation;
    int i=0;

    vector<string> att_typ;
    if(operation == "create"){
        schema.open("schema.txt",ios::out | ios::app);
        s>>operation;
        if(operation == "table"){
            string tmp;
            for(i=0;i<query.length();i++){
                if(query[i] == '(')
                    break;
            }
            i++;
            for(;i<query.length();i++){
                if(query[i] == ','){
                    att_typ.push_back(tmp);
                    tmp.clear();
                }else{
                    tmp.push_back(query[i]);
                }
            }
            att_typ.push_back(tmp);
        }else{
            cout<<"Invalid Query\n"<<endl;
            return ;
        }

        vector<string> individual_element;
        int att_typ_s = att_typ.size();
        for(i=0;i<att_typ_s;i++){
            string ext;
            individual_element.clear();
            for(int j=0;j<att_typ[i].length();j++){
                if(att_typ[i][j] == ' '){
                    individual_element.push_back(ext);
                    ext.clear();
                }else{
                    ext.push_back(att_typ[i][j]);
                }
            }
            if(i == att_typ_s-1)
                ext.pop_back();
            individual_element.push_back(ext);

            //Attribute name validation
            bool att_name = validate_att_name(individual_element[0]);
            if(!att_name){
                cout<<"Attribute name error\n"<<endl;
                return ;
            }

            //Attribute type validation
            bool att_type = validate_att_typ(individual_element[1]);
            if(!att_type){
                cout<<"Attribute type error\n"<<endl;
                return ;
            }
    }
    //Entry into schema
    stringstream tt(query);
    string table_col_in_table;
    string schema_entry;
    string tablename;
    tt>>tablename; //create
    tt>>tablename; // table
    tt>>tablename; //tablename

    schema_entry+=tablename;
    schema_entry+="#";

    //schema entry
    for(int i=0;i<att_typ_s;i++){
        stringstream t(att_typ[i]);
        string tmp;
        t>>tmp;
        tmp+="#";
        table_col_in_table+=tmp;
        schema_entry+=tmp;
        t>>tmp;
        tmp+="#";
        schema_entry+=tmp;
        tmp+="#";
    }
        schema_entry.erase(schema_entry.begin() + schema_entry.length() - 2);
        schema_entry+="\n";
        schema<<schema_entry;
        tablename+=".txt";
        table.open(tablename,ios::out);
        table<<table_col_in_table;
        table.close();
        cout<<"TABLE CREATED SUCCESSFULLY\n";
        schema.close();
    }else if(operation == "drop"){
        s>>operation;
        if(operation == "table"){
            s>>operation;

            string del=operation;
            //checking that file is already exists or not
            if(isTablePresent(del)){
                remove((del+".txt").c_str());

                //checking in schema.txt file and removing it
                fstream schFile;
                schFile.open("Temp.txt",ios::out);
                schema.open("schema.txt",ios::in);
                while(getline(schema,del)){
                    istringstream s(del);
                    string tmp;
                    getline(s,tmp,'#');
                    if(tmp!=operation){
                        del+="\n";
                        schFile<<del;
                    }
                }
                schema.close();
                remove("schema.txt");
                schFile.close();
                rename("Temp.txt","schema.txt");
                cout<<"Table dropped successfully\n";
            }else{
                cout<<"Table not Exists\n";
            }
        }else{
            cout<<"Invalid Query\n";
        }

    }else if(operation=="desc"){
        string tablename;
        s>>tablename;

        schema.open("schema.txt",ios::in);

        string line;
        string row;
        int flag=0; // for validation of table

        while(getline(schema,line)){
            istringstream s(line);
            string tmptable;
            getline(s,tmptable,'#');
            if(tmptable == tablename){
                flag=1;
                row=line;
                break;
            }
        }
        if(flag==1){
            istringstream r(line);
            string tmp;
            getline(r,tmp,'#');
            string descFormat;
            while(getline(r,tmp,'#')){
                descFormat+=tmp;
                descFormat+="\t";
                getline(r,tmp,'#');
                descFormat+=tmp;
                descFormat+="\n";
            }
            descFormat.pop_back();
            descFormat.pop_back();
            cout<<descFormat<<endl;
            cout<<endl;
        }else{
            cout<<"Table not found\n"<<endl;
        }
        schema.close();
    }else if(operation=="insert"){
        string keyword;
        s>>keyword;
        if(keyword == "into"){
            string tablename;
            s>>tablename;

            if(isTablePresent(tablename)){
                string val;
                s>>val;
                if(val=="values"){
                    int colcalculator = colCalculator(tablename);

                    vector<string> attributes_sequence = att_seq(tablename);


                    int givenColCal = 0;

                    string colString;
                    s>>colString;

                    colString.erase(colString.begin());
                    colString.pop_back();

                    if(valid(colString,attributes_sequence)){
                        istringstream col(colString);
                        istringstream t(colString);
                        string ttt;
                        while(getline(col,ttt,',')){
                            givenColCal++;
                        }

                        if(givenColCal == colcalculator){
                            string preparedString="\n";
                            while(getline(t,ttt,',')){
                                preparedString+=ttt;
                                preparedString+="#";
                            }
                            preparedString.pop_back();
                            table.open(tablename+".txt",ios::app | ios::out);
                            table<<preparedString;
                            table.close();
                            cout<<"Record Added Successfully\n"<<endl;
                        }else{
                            cout<<"You entered "<<givenColCal<<" arguments and there are "<<colcalculator<<" arguments in table\n"<<endl;
                        }
                    }else{
                        cout<<"Sequence not match with column sequence\n"<<endl;;
                    }
                }else{
                    cout<<"values keyword not found\n"<<endl;
                }

            }else{
                cout<<"Table not found\n"<<endl;
            }
        }else{
            cout<<"Invalid query\n"<<endl;
        }
        }else if(operation=="select"){
            string ttt,ttt1,ttt2,ttt3;
            s>>ttt;
            if(ttt=="*"){
                s>>ttt;
                s>>ttt;
                s>>ttt1;
                if(isTablePresent(ttt)) {

                    if(ttt1.compare("where") == 0) {

                        s>>ttt2;

                        if(ttt2[0] == 'i' && ttt2[1] == 'd') {

                            string s1 = ttt2.substr(3);

                            table.open(ttt+".txt",ios::in);
                            string records;
                            table>>records;
                            istringstream ss(records);
                            cout<<"---------------------------------------------------------------------"<<endl;
                            while(getline(ss,records,'#'))cout<<records<<"\t|\t";
                            cout<<"\n---------------------------------------------------------------------"<<endl;

                            while(getline(table,records)){
                                istringstream tmpss(records);
                                string id1;
                                getline(tmpss,id1,'#');
                                if(id1.compare(s1) == 0) {

                                    cout<<id1<<"\t|\t";
                                    string lis;
                                    while(getline(tmpss,lis,'#')){
                                        cout<<lis<<"\t|\t";
                                    }
                                    cout<<"\n---------------------------------------------------------------------"<<endl;
                                }

                            }
                            cout<<endl;
                            table.close();
                            }
                            else {

                                cout<<"Error, check syntax\n"<<endl;
                            }

                    }
                    else if(ttt1.compare("") == 0){
                        table.open(ttt+".txt",ios::in);
                        string records;
                        table>>records;
                        istringstream ss(records);
                        cout<<"---------------------------------------------------------------------"<<endl;
                        while(getline(ss,records,'#'))cout<<records<<"\t|\t";
                        //cout<<"\n---------------------------------------------------------------------";
                        while(getline(table,records)){
                            istringstream tmpss(records);
                            string lis;
                                while(getline(tmpss,lis,'#')){
                                    cout<<lis<<"\t|\t";
                                }
                                cout<<"\n---------------------------------------------------------------------"<<endl;
                        }
                        cout<<endl;
                        table.close();
                    }else {
                        cout<<"Invalid query\n"<<endl;
                    }

                }
                else{
                    cout<<"Table not exists\n"<<endl;
                }
            }else{

                vector<string> temp;
                istringstream col(ttt);
                string temp1;
                while(getline(col,temp1,',')){

                    temp.push_back(temp1);
                }

                string from; // from
                string ttt2; // table name // select id from emp
                s>>from;
                s>>ttt2;

                int flag = 0;
                if(isTablePresent(ttt2)){
                    table.open(ttt2+".txt",ios::in);
                    string records;
                    table>>records; // id#name#sal
                    istringstream ss(records);
                    vector<string> tempCol;
                    while(getline(ss,temp1,'#')){

                        tempCol.push_back(temp1);
                    }

                    int flag_ = 0;

                    for(int i = 0; i<temp.size(); i++) {

                        flag_ = 0;

                        for(int j = 0; j<tempCol.size(); j++) {

                            if(temp[i] == tempCol[j]) {

                                //cout<<temp[i]<<" "<<tempCol[j]<<" "<<flag_<<endl;
                                flag_ = 1;
                            }
                        }

                        if(flag_ == 0) {
                            cout<<"error, wrong column name"<<endl;
                            break;
                        }
                    }

                    if(flag_ != 0) {

                    vector<int> colnums;
                    (find(tempCol.begin(), tempCol.end(), "name") != tempCol.end());
                    for(int i = 0; i<temp.size(); i++){
                            for(int j = 0; j<tempCol.size(); j++){
                                if(temp[i] == tempCol[j]){
                                    colnums.push_back(j);
                                    //cout<<"\nInside if \n>"<<temp[i]<<"<----temp"<<"\n"<<tempCol[j]<<"<----tempcol if"<<"value of i = "<<i<<endl;
                                }else{
                                    //cout<<"\nInside else >\n"<<temp[i]<<"<----temp"<<"\n"<<tempCol[j]<<"<----tempcol else"<<"value of i = "<<i<<endl;
                                }
                        }
                    }
                    cout<<"\n----------------------------------------------------------------------------------------------"<<endl;
                    for(int i = 0; i<temp.size(); i++) {
                        cout<<"\t"<<temp[i]<<"\t|";
                    }
                    cout<<"\n----------------------------------------------------------------------------------------------"<<endl;

                    while(getline(table,records)){
                        istringstream tmpss(records);
                        string lis;
                        int check = 0;
                        int counter = 0;
                            while(getline(tmpss,lis,'#')){
                                if(find(colnums.begin(), colnums.end(), check) != colnums.end()){
                                    cout<<"\t"<<lis<<"\t|";
                                    counter++;
                                    if(counter == temp.size()){
                                        cout<<"\n----------------------------------------------------------------------------------------------"<<endl;
                                    }

                                }
                                check++;
                            }
                    }

                    cout<<endl;

                    table.close();
                    }
                    }else{
                    cout<<"Table is not present\n"<<endl;
                }


            }
        }else if(operation=="delete"){
            string ttt;
            s>>ttt;
            string tx;
            s>>ttt;
            s>>tx;
            //cout<<"tx: "<<tx<<endl;

            if(isTablePresent(ttt)){
                    if(tx==""){
                        table.open(ttt+".txt",ios::in);
                        string tx;
                        table>>tx;
                        table.close();
                        remove((ttt+".txt").c_str());
                        table.open(ttt+".txt",ios::out | ios::app);
                        table<<tx;
                        table<<"\n";
                        table.close();
                        cout<<"Table cleared successfully!!\n"<<endl;
                    }else if(tx=="where"){
                        string tablename=ttt;
                        s>>ttt; //where
                        s>>ttt; //id=???

                        string id;

                        // Id extraction
                        id = ttt.substr(3);
                        // P - Record Present, A - Deleted Record
                        table.open(tablename+".txt",ios::in);
                        fstream tmp;
                        tmp.open("tmp.txt",ios::app | ios::out);
                        string rec;
                        getline(table,rec);
                        tmp<<rec;

                        while(getline(table,rec)){
                            istringstream ist(rec);
                            string r;
                            getline(ist,r,'#');
                            if(r!=id){
                                tmp<<"\n";
                                tmp<<rec;
                            }
                        }
                        tmp.close();
                        table.close();
                        tablename+=".txt";
                        remove(tablename.c_str());
                        rename("tmp.txt",tablename.c_str());
                        cout<<"Record deleted successfully\n"<<endl;
                    }else{
                        cout<<"Invalid query!!\n"<<endl;
                    }
            }else{
                cout<<"Invalid query!! Table not exists\n"<<endl;
            }


        }else if(operation=="update"){
            string tablename;
            s>>tablename;
            if(isTablePresent(tablename)){

                string st;
                s>>st;
                if(st=="set"){
                    s>>st;
                    st.erase(st.begin());
                    st.pop_back();
                    string tmpS;
                    s>>tmpS; //where
                    s>>tmpS; //id

                    string id = tmpS.substr(3);

                    table.open(tablename+".txt",ios::in);
                    fstream tmp;
                    tmp.open("tmp.txt",ios::out);
                    string rec,idss,preparedString,tss;
                    getline(table,rec);
                    tmp<<rec;
                    while(getline(table,rec)){
                        istringstream stt(rec);
                        getline(stt,idss,'#');
                        if(idss==id){
                            istringstream tist(st);
                            while(getline(tist,tss,',')){
                                preparedString+=tss;
                                preparedString+="#";
                            }
                            tmp<<"\n";
                            tmp<<preparedString;
                        }else{
                            tmp<<"\n";
                            tmp<<rec;
                        }
                    }
                    tmp.close();
                    table.close();
                    tablename+=".txt";
                    remove(tablename.c_str());
                    rename("tmp.txt",tablename.c_str());
                    cout<<"Record updated successfully\n"<<endl;
                }else{
                    cout<<"Invalid command\n"<<endl;
                }

            }else{
                cout<<"Table not present\n"<<endl;
            }



        }else if(operation=="help"){
            string tmp;
            s>>tmp;
            if(tmp=="cmd"){
                cout<<endl;
                cout<<"\t1. CREATE TABLE.\n\t   create table tablename (param1 datatype,param2 datatype,......);\n"<<endl;
                cout<<"\t2. DROP TABLE.\n\t   drop table tablename;\n"<<endl;
                cout<<"\t3. DESCRIBE TABLE.\n\t   desc tablename;\n"<<endl;
                cout<<"\t4. INSERT RECORD.\n\t   insert into tablename (value1,value2,value3,........);\n"<<endl;
                cout<<"\t5. DELETE RECORD.\n\t   delete from tablename where id=value;\n"<<endl;
                cout<<"\t6. UPDATE RECORD.\n\t   update tablename set (value1,value2,value3,........) where id=value;\n"<<endl;
                cout<<"\t7. DISPLAY RECORDS.\n\t   select * from tablename;\n"<<endl;
                cout<<"\t8. HELP TABLES.\n\t   help tables;\n"<<endl;
                cout<<"\t9. HELP CMD.\n\t   help cmd;\n"<<endl;
                cout<<"\t10. RENAME TABLE.\n\t   rename table present_table_name to desired_table_name;\n"<<endl;
            }else if(tmp=="tables"){
                schema.open("schema.txt", ios::in);

                if (schema.is_open()){
                    string tp;
                    int i=0;
                    while(getline(schema, tp)){
                        stringstream s(tp);
                        string table_name;
                        getline(s,table_name,'#');
                        cout<<(i+1)<<". "<<table_name<<endl;
                    }
                }
                cout<<endl;
                schema.close();
            }else if(tmp=="create"){
                cout<<endl;
                cout<<"\tCREATE TABLE.\n\t   create table tablename (param1 datatype,param2 datatype,......);\n"<<endl;
            }
            else if(tmp=="drop"){
                cout<<endl;
                cout<<"\tDROP TABLE.\n\t   drop table tablename;\n"<<endl;
            }else if(tmp=="desc"){
                cout<<endl;
                cout<<"\tDESCRIBE TABLE.\n\t   desc tablename;\n"<<endl;
            }else if(tmp=="insert"){
                cout<<endl;
                cout<<"\tINSERT RECORD.\n\t   insert into tablename (value1,value2,value3,........);\n"<<endl;
            }else if(tmp=="delete"){
                cout<<endl;
                cout<<"\tDELETE RECORD.\n\t   delete from tablename where id=value;\n"<<endl;
            }else if(tmp=="update"){
                cout<<endl;
                cout<<"\tUPDATE RECORD.\n\t   update tablename set (value1,value2,value3,........) where id=value;\n"<<endl;
            }else if(tmp=="display"){
                cout<<endl;
                cout<<"\tDISPLAY RECORDS.\n\t   select * from tablename;\n"<<endl;
            }
            else{
                cout<<"Invalid command!!\n"<<endl;
            }
        }else if(operation=="rename"){
            string tablename,desired_name;;
            s>>tablename;
            s>>tablename;
            s>>desired_name;
            s>>desired_name;
            if(isTablePresent(tablename)){
                schema.open("schema.txt",ios::in);
                fstream tmp;
                string rec,rec_s;
                tmp.open("tmp.txt",ios::out | ios::app);
                while(getline(schema,rec)){
                    istringstream r(rec);
                    getline(r,rec_s,'#');
                    if(rec_s == tablename){

                        string preString=desired_name;
                        while(getline(r,rec_s,'#')){
                            preString+="#";
                            preString+=rec_s;
                        }
                        tmp<<preString;
                        tmp<<"\n";
                    }else{
                        tmp<<rec;
                        tmp<<"\n";
                    }
                }
                tmp.close();
                schema.close();
                remove("schema.txt");
                rename("tmp.txt","schema.txt");
                rename((tablename+".txt").c_str(),(desired_name+".txt").c_str());
                cout<<"Table renamed successfully\n"<<endl;
            }else{
                cout<<"Invalid Tablename!! Table not present\n"<<endl;
            }
        }else{
            cout<<"Invalid query!!!\n"<<endl;
        }
}


int main(){
    int ch=1;
    while(ch!=2){
		cout<<"1.Make Query\n2.Exit: "<<endl;
		cout<<"Enter the Choice: ";
		cin>>ch;
		switch(ch){
			case 1:
				query_processing();
				break;
			case 2:
				exit(0);
		}
	}
}

