#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include<queue>

#include <cmath>
#include <iterator>
#include <list>
#include <string>
#include <type_traits>


using namespace std;

struct Coordinate{
    float x, y;
};

struct Instance {
    string name;
    string type;
    Coordinate* center;
    Instance* next;
    vector<int> belongedNet;
    vector<int> belongedNetSpecial;
    Instance() : name("") {
        
    }
};

struct Net {
    vector<string> connectedInstances;
};

struct CLBResource {
    string name;
    Coordinate* center;
    int isUsed;//0->negative,1->positive
};

struct ComplexResource {
    string name;
    Coordinate* center;
    int isUsed;//0->negative,1->positive
};


struct ComplexBlockResourceInfo {
    string name;
    Coordinate* center;
    int isUsed;//0->negative,1->positive
    ComplexBlockResourceInfo* nextResource;
};

struct ComplexResourceArray{
    ComplexBlockResourceInfo* Resource;
};

void getXY(int &a, int &b,string arch_path){
    ifstream inputFile(arch_path);

    string line;
    float maxX = numeric_limits<float>::lowest();
    float maxY = numeric_limits<float>::lowest();

    while (getline(inputFile, line)) {
        istringstream iss(line);
        string name, type;
        float x, y;  

        if (!(iss >> name >> type >> x >> y)) {
            cerr << "Error reading line: " << line << endl;
            continue;
        }

        
        maxX = max(maxX, x);
        maxY = max(maxY, y);
    }

    inputFile.close();

    a = (int)(maxX+0.5);
    b = (int)(maxY+0.5);
}


void getCLBResource(CLBResource** ArchCLB,int i,int j, string name){
    ArchCLB[i][j].name = name; 
    ArchCLB[i][j].isUsed = 0;
}

void getRAMResource(ComplexResource** ArchRAM, int i,int j,float x,float y, string name){
    ArchRAM[i][j].name = name; 
    ArchRAM[i][j].isUsed = 0;
    Coordinate * coord = new Coordinate();
    coord->x = x;
    coord->y = y;
    ArchRAM[i][j].center = coord;
}

void getDSPResource(ComplexResource** ArchDSP, int i,int j,float x,float y, string name){
    
    ArchDSP[i][j].name = name; 
    ArchDSP[i][j].isUsed = 0;

    Coordinate * coord = new Coordinate();
    coord->x = x;
    coord->y = y;
    ArchDSP[i][j].center = coord;
}

void getResources(CLBResource **ArchCLB,ComplexResource** ArchRAM,ComplexResource** ArchDSP,int rows,int columns,int RAM_cols,int DSP_cols,string arch_path){

    int current_RAM_i = 0;
    int current_DSP_i = 0;

    ifstream inputFile(arch_path);

    string line;

    while (getline(inputFile, line)) {
        istringstream iss(line);
        string name, type;
        float x, y;
        int i,j;

        if (iss >> name >> type >> x >> y) {
            if(type == "CLB"){
                i = y-0.5;
                j = x-0.5;
                getCLBResource(ArchCLB,rows-i-1,j,name);

            }
            else if(type == "RAM"){
               
                j = x-0.5;

                getRAMResource(ArchRAM,current_RAM_i,j,x,y,name);

                current_RAM_i++;
                if(current_RAM_i == RAM_cols) current_RAM_i = 0;
            }
            else if(type == "DSP"){
                
                j = x-0.5;
                getDSPResource(ArchDSP,current_DSP_i,j,x,y,name);

                current_DSP_i++;
                if(current_DSP_i == DSP_cols) current_DSP_i = 0;
            }
            
            
        
        } else {
            cerr << "Error parsing line: " << line << endl;
        }
    }

    inputFile.close();

}

Instance* makeInstance(string name,string type,float x,float y){
    Instance* instance = new Instance();
    instance->name = name;
    instance->type = type;
    Coordinate *coord = new Coordinate();
    coord->x = x;
    coord->y = y;
    instance->center = coord;
    instance->next = nullptr;
    return instance;
}

void getAllInstance(Instance** CLBInstance,Instance** otherInstance,vector<vector<float>>& CLB_center,vector<vector<float>>& RAM_center,vector<vector<float>>& DSP_center,string instance_path){
    Instance* instaPtr = nullptr;

    ifstream inputFile(instance_path);

    string line;

    while (getline(inputFile, line)) {
        istringstream iss(line);
        string name, type;
        float x, y;

        if (iss >> name >> type >> x >> y) {
            
            Instance* instance = makeInstance(name,type,x,y);
            
            
            if (name.substr(0, 3) == "CLB") {
                

                
                
                
                size_t underscorePos = line.find('B');
                int index = 0;
    
                if (underscorePos != string::npos) {
                // 提取底線之後的字串
                    string numberStr = line.substr(underscorePos + 1);

                    try {
                      // 將提取的字串轉換為整數
                       int number = stoi(numberStr);
                       index = number;
                       CLBInstance[index] = instance;

                       float center_distance = x + y;


                       CLB_center[0].push_back(index);
                       CLB_center[1].push_back(center_distance);
                      
                       
                    } catch (const invalid_argument& e) {
                         cerr << "Invalid argument: " << e.what() << endl;
                    } catch (const out_of_range& e) {
                        cerr << "Out of range: " << e.what() << endl;
                    }

                    
                }
                
            } else if (name.substr(0, 4) == "inst") {
                
                
                size_t underscorePos = line.find('_');
                int index = 0;
    
                if (underscorePos != string::npos) {
                // 提取底線之後的字串
                    string numberStr = line.substr(underscorePos + 1);

                    try {
                      // 將提取的字串轉換為整數
                       int number = stoi(numberStr);
                       index = number;
                       
                    } catch (const invalid_argument& e) {
                         cerr << "Invalid argument: " << e.what() << endl;
                    } catch (const out_of_range& e) {
                        cerr << "Out of range: " << e.what() << endl;
                    }
                }
               
                otherInstance[index] = instance;

                float center_distance = x + y;

                if(type == "RAM"){
                    RAM_center[0].push_back(index);
                    RAM_center[1].push_back(center_distance);
                }
                else if(type == "DSP"){
                    DSP_center[0].push_back(index);
                    DSP_center[1].push_back(center_distance);
                }
              
                
            }

            
            
            
            
        } else {
            
            cerr << "Error parsing line: " << line << endl;
        }
    }

    inputFile.close();

    
    

}

void readNetData(Net*& netArray, int& numNets, const string& filename) {
    ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open file." << endl;
        exit(EXIT_FAILURE);
    }

    string line;

    // 計算檔案行數
    while (getline(inputFile, line)) {
        ++numNets;
    }

    // 回到檔案開頭
    inputFile.clear();
    inputFile.seekg(0, ios::beg);

    // 建立動態陣列
    netArray = new Net[numNets];


    // 關閉檔案
    inputFile.close();
 
}

void addSpecialCaseList(Net *specialCaseArray,string token, int i,Instance** CLBArray,Instance** otherArray){
    specialCaseArray[i].connectedInstances.push_back(token);
                if (token.substr(0, 3) == "CLB") {
                // 如果開頭是 CLB，則截取 'B' 之後的數字
                string numberStr = token.substr(3);
                try {
                    int number = stoi(numberStr);
                    CLBArray[number]->belongedNetSpecial.push_back(i);
                    // cout<<token<<" "<<i<<" "<<endl;

                    
                
                } catch (const invalid_argument& e) {
                    cerr << "Invalid CLB number: " << numberStr << endl;
                }
                } else if (token.substr(0, 4) == "inst") {
                // 如果開頭是 inst，則截取 '_' 之後的數字
                size_t underscorePos = token.find('_');
                if (underscorePos != string::npos) {
                    string numberStr = token.substr(underscorePos + 1);
                    try {
                        int number = stoi(numberStr);
                        otherArray[number]->belongedNetSpecial.push_back(i);
                        // cout<<token<<" "<<i<<" "<<endl;
                        
                    } catch (const invalid_argument& e) {
                        cerr << "Invalid instance number: " << numberStr << endl;
                    }
                } else {
                    cerr << "Invalid inst token format: " << token << endl;
                }
                }
}


void makeNetList(Net* netArray,Net* specialCaseArray,int& index_of_SPC,Instance** CLBArray,Instance** otherArray,string netlist_path){


    ifstream inputFile(netlist_path);

    string line;

    while (getline(inputFile, line)) {
        istringstream iss(line);
        string token;
        string key;
        if (iss >> key) {
            // key = key.substr(key.length() - 4);
            // cout<<key<<endl;
            // int index = stoi(key);

            if (key.substr(0, 3) != "net") {
                while (iss >> token) {
                    addSpecialCaseList(specialCaseArray,token,index_of_SPC,CLBArray,otherArray);
                }
                index_of_SPC++;
            }

            else{

            size_t underscorePos = line.find('_');
            int index = 0;
    
            if (underscorePos != string::npos) {
                
            // 提取底線之後的字串
                string numberStr = line.substr(underscorePos + 1);

                try {
                 // 將提取的字串轉換為整數
                    int number = stoi(numberStr);
                    index = number;
                } catch (const invalid_argument& e) {
                    cerr << "Invalid argument: " << e.what() << endl;
                    // cout<<key<<endl;
                } catch (const out_of_range& e) {
                    cerr << "Out of range: " << e.what() << endl;
                }
            }

            // 將連接的實例加入Net
            
            while (iss >> token) {
                netArray[index].connectedInstances.push_back(token);

                if (token.substr(0, 3) == "CLB") {
                // 如果開頭是 CLB，則截取 'B' 之後的數字
                string numberStr = token.substr(3);
                try {
                    int number = stoi(numberStr);
                    CLBArray[number]->belongedNet.push_back(index);

                    
                
                } catch (const invalid_argument& e) {
                    cerr << "Invalid CLB number: " << numberStr << endl;
                }
                } else if (token.substr(0, 4) == "inst") {
                // 如果開頭是 inst，則截取 '_' 之後的數字
                size_t underscorePos = token.find('_');
                if (underscorePos != string::npos) {
                    string numberStr = token.substr(underscorePos + 1);
                    try {
                        int number = stoi(numberStr);
                        otherArray[number]->belongedNet.push_back(index);
                        
                    } catch (const invalid_argument& e) {
                        cerr << "Invalid instance number: " << numberStr << endl;
                    }
                } else {
                    cerr << "Invalid inst token format: " << token << endl;
                }
                }
            }
            }
        }
    }

    inputFile.close();
    
    
    
 
}

void addNetList(Net* net,string name){
    net->connectedInstances.push_back(name);
}

float HPWL_e(Net *net,Instance** CLBArray,Instance** otherArray){
    float maxX = numeric_limits<float>::lowest();
    float maxY = numeric_limits<float>::lowest();
    float minX = numeric_limits<float>::infinity();
    float minY = numeric_limits<float>::infinity();
    for (const auto &element : net->connectedInstances) {
        if (element.substr(0, 3) == "CLB") {
           
            // 如果開頭是 CLB，則截取 'B' 之後的數字
            string numberStr = element.substr(3);
           
            int number = stoi(numberStr);
            
            maxX = max(CLBArray[number]->center->x,maxX);
            maxY = max(CLBArray[number]->center->y,maxY);
            minX = min(CLBArray[number]->center->x,minX);
            minY = min(CLBArray[number]->center->y,minY);
                    
                    
                
                
        } else if (element.substr(0, 4) == "inst") {
            // 如果開頭是 inst，則截取 '_' 之後的數字
           
                size_t underscorePos = element.find('_');
                if (underscorePos != string::npos) {
                    string numberStr = element.substr(underscorePos + 1);
                    
                    int number = stoi(numberStr);


                    maxX = max(otherArray[number]->center->x,maxX);
                    maxY = max(otherArray[number]->center->y,maxY);
                    minX = min(otherArray[number]->center->x,minX);
                    minY = min(otherArray[number]->center->y,minY);
                    
                        
                        
                    
                } else {
                    cerr << "Invalid inst token format: " << element << endl;
                }
        }
    }
    
   

    return maxX-minX+maxY-minY;
        
    
    

}

// 计算两个数的绝对值差
double absoluteDifference(double a, double b) {
    return abs(a - b);
}

// 函数：选择与 x 相差最小的三个元素
vector<int> selectClosestElements(double x, const vector<int>& y) {
    // 创建一个副本以保留原始元素顺序
    vector<int> yCopy = y;

    // 使用 lambda 函数根据与 x 的绝对值差进行排序
    sort(yCopy.begin(), yCopy.end(), [x](int a, int b) {
        return absoluteDifference(a, x) < absoluteDifference(b, x);
    });

    // 取最小的三个元素
    vector<int> result(yCopy.begin(), yCopy.begin() + min(2, static_cast<int>(yCopy.size())));

    return result;
}


void makeBipartite(Net* netArray,Net* specialCaseArray,Instance **CLBArray,Instance** otherArray,CLBResource** ArchCLB,ComplexResourceArray** ArchDSP,ComplexResourceArray** ArchRAM,int x,vector<string> &RAM_left,vector<string> &RAM_right,vector<string> &DSP_left,vector<string> &DSP_right,vector<vector<int>>& RAM_B,vector<vector<int>>& DSP_B,vector<Coordinate*>& RAM_new_center,vector<Coordinate*>& DSP_new_center,vector<int>& nearest_position_RAM,vector<int>& nearest_position_DSP){

    Coordinate* origin_center;

    int RAM_left_index =0;

    int DSP_left_index =0;


    int flag1 = 0;
    int flag2 = 0;

    

    for(int i=0;i<1000000;i++){
        if(otherArray[i] != nullptr && otherArray[i]->type == "RAM"){
           
            RAM_left.push_back(otherArray[i]->name);
            RAM_B.push_back(vector<int>());


            vector<int> closestElements = selectClosestElements(otherArray[i]->center->x, nearest_position_RAM);

     

            for (int position : closestElements) {

                   

                    ComplexBlockResourceInfo* temp = ArchRAM[position]->Resource;
                    while(temp != nullptr){
                      

                        
                        

                        origin_center = otherArray[i]->center;
                        otherArray[i]->center = temp->center;
                        float HPWL = 0;
                        for (const auto &element : otherArray[i]->belongedNet) {
                            
                            HPWL += HPWL_e(&(netArray[element]),CLBArray,otherArray);
     
                        }

                        for (const auto &element : otherArray[i]->belongedNetSpecial) {
                            
                            HPWL += HPWL_e(&(specialCaseArray[element]),CLBArray,otherArray);
     
                        }

                        RAM_B[RAM_left_index].push_back((int)HPWL);
                        


                        
                        

                        if(flag1 == 0){
                            RAM_right.push_back(temp->name);
                            RAM_new_center.push_back(temp->center);
                        }


                        

                        
                        otherArray[i]->center = origin_center;
                        temp = temp->nextResource;
                    
                    }
                   
                
                
            }



            RAM_left_index++;

            flag1 = 1;
        }





        if(otherArray[i] != nullptr && otherArray[i]->type == "DSP"){
           
           
            DSP_left.push_back(otherArray[i]->name);
            DSP_B.push_back(vector<int>());

           

            vector<int> closestElements = selectClosestElements(otherArray[i]->center->x, nearest_position_DSP);

            for (int position : closestElements) {

                    ComplexBlockResourceInfo* temp = ArchDSP[position]->Resource;
                    while(temp != nullptr){

                        
                        

                        origin_center = otherArray[i]->center;
                        otherArray[i]->center = temp->center;
                        float HPWL = 0;
                        for (const auto &element : otherArray[i]->belongedNet) {
                            
                            HPWL += HPWL_e(&(netArray[element]),CLBArray,otherArray);
     
                        }

                        for (const auto &element : otherArray[i]->belongedNetSpecial) {
                            
                            HPWL += HPWL_e(&(specialCaseArray[element]),CLBArray,otherArray);
     
                        }

                        DSP_B[DSP_left_index].push_back((int)HPWL);
                        


                        
                        

                        if(flag2 == 0){
                            DSP_right.push_back(temp->name);
                            DSP_new_center.push_back(temp->center);
                        }


                        

                        
                        otherArray[i]->center = origin_center;
                        temp = temp->nextResource;
                    
                    }
                
                
            }



            DSP_left_index++;

            flag2 = 1;
        }



    }

}

// 定義比較函數，用於 sort
bool compare(const pair<float, int>& a, const pair<float, int>& b) {
    return a.first < b.first;
}

// 函數對 A[1] 進行排序，同時保持 A[0] 對應順序
void sortArray(vector<vector<float>>& A) {
    const int columns = A[1].size();

    // 創建一個輔助陣列，存儲 A[1] 的值和對應的 A[0] 的索引
    vector<pair<float, int>> temp;

    // 將 A[1] 的值和對應的 A[0] 的索引存入輔助陣列
    for (int j = 0; j < columns; ++j) {
        temp.push_back({A[1][j], A[0][j]});
    }

    // 使用 sort 對輔助陣列排序，根據 A[1] 的值
    sort(temp.begin(), temp.end(), compare);

    // 根據排序後的索引更新 A[1] 和 A[0]
    for (int j = 0; j < columns; ++j) {
        A[1][j] = temp[j].first;
        A[0][j] = temp[j].second;
    }
}

struct Index {
    int row;
    int col;
};


// 检查索引是否在矩阵边界内
bool isValidIndex(int rows,int cols, int row, int col) {
    return (row >= 0 && row < rows && col >= 0 && col < cols);
}

// 执行广度优先搜索
Index bfs_Complex(ComplexResource** ArchComplex, int startRow, int startCol, int rows,int cols) {


    queue<Index> q;
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));

    q.push({startRow, startCol});
    visited[startRow][startCol] = true;

    while (!q.empty()) {
        Index current = q.front();
        q.pop();

        // 检查当前位置是否为0
        if (ArchComplex[current.row][current.col].isUsed == 0) {
            return current;
        }

        // 检查相邻的元素
        const int dx[] = {-1, 1, 0, 0};
        const int dy[] = {0, 0, -1, 1};

        for (int i = 0; i < 4; ++i) {
            int newRow = current.row + dx[i];
            int newCol = current.col + dy[i];

            // 检查相邻元素的有效性和是否已访问
            if (isValidIndex(rows,cols, newRow, newCol) && !visited[newRow][newCol]) {
                q.push({newRow, newCol});
                visited[newRow][newCol] = true;
            }
        }
    }

    // 如果未找到元素值为0的元素，返回{-1, -1}
    return {-1, -1};
}

// 执行广度优先搜索
Index bfs(CLBResource** ArchCLB, int startRow, int startCol, int rows,int cols) {


    queue<Index> q;
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));

    q.push({startRow, startCol});
    visited[startRow][startCol] = true;

    while (!q.empty()) {
        Index current = q.front();
        q.pop();

        // 检查当前位置是否为0
        if (ArchCLB[current.row][current.col].isUsed == 0) {
            return current;
        }

        // 检查相邻的元素
        const int dx[] = {-1, 1, 0, 0};
        const int dy[] = {0, 0, -1, 1};

        for (int i = 0; i < 4; ++i) {
            int newRow = current.row + dx[i];
            int newCol = current.col + dy[i];

            // 检查相邻元素的有效性和是否已访问
            if (isValidIndex(rows,cols, newRow, newCol) && !visited[newRow][newCol]) {
                q.push({newRow, newCol});
                visited[newRow][newCol] = true;
            }
        }
    }

    // 如果未找到元素值为0的元素，返回{-1, -1}
    return {-1, -1};
}

void CLB_Legalization(vector<vector<float>>& CLB_center,CLBResource** ArchCLB,Instance** CLBArray,int rows,int cols, vector<string>& placement_result){
    for(int i=0;i<CLB_center[0].size();i++){

        int index = (int)CLB_center[0][i];
        float x = CLBArray[index]->center->x;
        float y = CLBArray[index]->center->y;



        x = floor(x);
        y = floor(y);

        int index_i = rows-y-1;
        int index_j = x;


        
        // 执行广度优先搜索
        Index result = bfs(ArchCLB, index_i, index_j,rows,cols);
   

         // 输出结果
        if (result.row != -1 && result.col != -1) {

            ArchCLB[result.row][result.col].isUsed = 1;
      

            x = (float)result.col + 0.5;
            y = (float)rows - (float)result.row - 1.5;

          

            CLBArray[index]->center->x = x;
            CLBArray[index]->center->y = y;

      

            

            


            placement_result.push_back(CLBArray[index]->name + " " + ArchCLB[result.row][result.col].name);
           
            
        } 
        
        

    }
}

void RAM_Legalization(vector<vector<float>>& complex_center,ComplexResource** ArchComplex,Instance** otherArray,int RAM_y,int rows,int cols, vector<string>& placement_result){
    

    for(int i=0;i<complex_center[0].size();i++){
        int index = (int)complex_center[0][i];
        float x = otherArray[index]->center->x;
        float y = otherArray[index]->center->y;

        int index_i = (rows-floor(y))/5;
        int index_j = floor(x);
        
        // 执行广度优先搜索
        Index result = bfs_Complex(ArchComplex, index_i, index_j,RAM_y,cols);

         // 输出结果
        if (result.row != -1 && result.col != -1) {
            

            ArchComplex[result.row][result.col].isUsed = 1;

            float new_x =  ArchComplex[result.row][result.col].center->x;
            float new_y =  ArchComplex[result.row][result.col].center->y;

            otherArray[index]->center->x = new_x;
            otherArray[index]->center->y = new_y;

            placement_result.push_back(otherArray[index]->name + " " + ArchComplex[result.row][result.col].name);

            
        } 
        
        

    }
    

}

void DSP_Legalization(vector<vector<float>>& complex_center,ComplexResource** ArchComplex,Instance** otherArray,int DSP_y,int rows,int cols, vector<string>& placement_result){
    

    for(int i=0;i<complex_center[0].size();i++){
        int index = (int)complex_center[0][i];
        float x = otherArray[index]->center->x;
        float y = otherArray[index]->center->y;


        int index_i = (rows-floor(y))/2.5;
        int index_j = floor(x);
        
        // 执行广度优先搜索
        Index result = bfs_Complex(ArchComplex, index_i, index_j,DSP_y,cols);

         // 输出结果
        if (result.row != -1 && result.col != -1) {
            

            ArchComplex[result.row][result.col].isUsed = 1;

            float new_x =  ArchComplex[result.row][result.col].center->x;
            float new_y =  ArchComplex[result.row][result.col].center->y;

            otherArray[index]->center->x = new_x;
            otherArray[index]->center->y = new_y;

            placement_result.push_back(otherArray[index]->name + " " + ArchComplex[result.row][result.col].name);

            
        } 
        
        

    }
   

}

int get_RAM_cols(string arch_path){
    ifstream inputFile(arch_path);  // 替换为你的文件名

    if (!inputFile.is_open()) {
        cerr << "Unable to open the file.\n";
        return 1;
    }

    string line;
    int ramBlockLength = 0;
    bool insideRamBlock = false;

    while (getline(inputFile, line)) {

        istringstream iss(line);
        string name, type;
        float x, y;
        int i,j;

        if (iss >> name >> type >> x >> y) {
            if(type == "RAM"){
                insideRamBlock = true;
                ramBlockLength++;  // 从当前行开始计算长度
            }
            if(insideRamBlock && type != "RAM"){
                
                inputFile.close();

                return ramBlockLength;
            }
        }
    }

   

    inputFile.close();
}

int get_DSP_cols(string arch_path){
    ifstream inputFile(arch_path);  // 替换为你的文件名

    if (!inputFile.is_open()) {
        cerr << "Unable to open the file.\n";
        return 1;
    }

    string line;
    int dspBlockLength = 0;
    bool insideDspBlock = false;

    while (getline(inputFile, line)) {

        istringstream iss(line);
        string name, type;
        float x, y;
        int i,j;

        if (iss >> name >> type >> x >> y) {
            if(type == "DSP"){
                insideDspBlock = true;
                dspBlockLength++;  // 从当前行开始计算长度
            }
            if(insideDspBlock && type != "DSP"){
                
                inputFile.close();

                return dspBlockLength;
            }
        }
    }

   

    inputFile.close();
}





int main(int argc, char *argv[]) {
   

    string arch_path = argv[1];
    string instance_path = argv[2];
    string netlist_path = argv[3];
    string output_path = argv[4];
    
  

    vector<string> placement_result;

    int x,y;

    getXY(x,y,arch_path);

    int RAM_y = get_RAM_cols(arch_path);
    int DSP_y = get_DSP_cols(arch_path);



    CLBResource** ArchCLB = new CLBResource*[y];
    ComplexResource** ArchRAM = new ComplexResource*[RAM_y];
    ComplexResource** ArchDSP = new ComplexResource*[DSP_y];
    for(int i=0;i<y;i++){
        ArchCLB[i] = new CLBResource[x];
    }

    

    for(int i=0;i<y;i++){
        for(int j=0;j<x;j++){
            ArchCLB[i][j].name = "";
            ArchCLB[i][j].isUsed = 1;
        }
        
    }

    for(int i=0;i<RAM_y;i++){
        ArchRAM[i] = new ComplexResource[x];
    }

    for(int i=0;i<RAM_y;i++){
        for(int j=0;j<x;j++){
            ArchRAM[i][j].name = "";
            ArchRAM[i][j].isUsed = 1;
        }
        
    }

    for(int i=0;i<DSP_y;i++){
        ArchDSP[i] = new ComplexResource[x];
    }

    for(int i=0;i<DSP_y;i++){
        for(int j=0;j<x;j++){
            ArchDSP[i][j].name = "";
            ArchDSP[i][j].isUsed = 1;
        }
        
    }

    cout<<"Getting Architecture file.."<<endl;




    // ComplexResourceArray** ArchRAM = new ComplexResourceArray*[x];
    // for (int i = 0; i < x; i++) {
    //     ArchRAM[i] = new ComplexResourceArray();
    //     ArchRAM[i]->Resource = nullptr;
    // }

    // ComplexResourceArray** ArchDSP = new ComplexResourceArray*[x];
    // for (int i = 0; i < x; i++) {
    //     ArchDSP[i] = new ComplexResourceArray();
    //     ArchDSP[i]->Resource = nullptr;
    // }

    getResources(ArchCLB,ArchRAM,ArchDSP,y,x,RAM_y,DSP_y,arch_path);

    cout<<"Getting Architecture file done"<<endl;

    





    Instance** CLBArray = new Instance*[1000000];
    Instance** otherArray = new Instance*[1000000];

    for(int i=0;i<1000000;i++){
        CLBArray[i] = nullptr;
        otherArray[i] = nullptr;
    }


    vector<vector<float>> CLB_center(2);

    vector<vector<float>> RAM_center(2);

    vector<vector<float>> DSP_center(2);


    cout<<"Getting Instance file..."<<endl;





   
    getAllInstance(CLBArray,otherArray,CLB_center,RAM_center,DSP_center,instance_path);
    

    sortArray(CLB_center);

    sortArray(RAM_center);

    sortArray(DSP_center);

    cout<<"Getting Instance file done"<<endl;



    

 
    

    


    // Net* netArray = new Net[10000000];
    // Net* specialCaseArray = new Net[10000000];
    // int numNets = 0;          // 陣列長度
    // int index_of_SPC = 0;

  
    
    // makeNetList(netArray,specialCaseArray,index_of_SPC,CLBArray,otherArray,netlist_path);

    
    cout<<"start legalizing..."<<endl;

    RAM_Legalization(RAM_center,ArchRAM,otherArray,RAM_y,y,x,placement_result);
    cout<<"RAM Legalization done"<<endl;
    
    DSP_Legalization(DSP_center,ArchDSP,otherArray,DSP_y,y,x,placement_result);
    cout<<"DSP Legalization done"<<endl;
    
  

    CLB_Legalization(CLB_center,ArchCLB,CLBArray,y,x,placement_result);
    cout<<"CLB Legalization done"<<endl;




    ofstream outputFile(output_path);
    if (outputFile.is_open()) {
        for (const string& result : placement_result) {
            outputFile << result<<endl;;
        }

        // 关闭文件
        outputFile.close();
      
    } 

    cout<<"All done";
    


 
    return 0;
}