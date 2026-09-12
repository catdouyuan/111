#include <bits/stdc++.h>
using namespace std;

int n, k, q;

vector<vector<int>> img; // 当前图片内容，编号从1开始
vector<int> order;       // 从下到上
vector<int> posi;        // 图片所在层

vector<vector<unsigned long long>> maskCell; 
vector<int> topImg;      // 每个格子当前显示图片

int words;

// 判断第x张图是否在该格子透明
bool hasImg(int id, int cell) {
    int w = (id - 1) >> 6;
    int b = (id - 1) & 63;
    return (maskCell[cell][w] >> b) & 1ULL;
}

// 设置某格子的某图片状态
void setImg(int id, int r, int c, bool val) {
    int cell = r * n + c;
    int w = (id - 1) >> 6;
    int b = (id - 1) & 63;

    if (val)
        maskCell[cell][w] |= (1ULL << b);
    else
        maskCell[cell][w] &= ~(1ULL << b);
}

// 找某个格子当前最高的不透明图片
int getTop(int cell) {
    for (int i = k - 1; i >= 0; i--) {
        int id = order[i];
        if (hasImg(id, cell))
            return id;
    }
    return 0;
}

// 更新一个格子的显示结果
void refresh(int cell) {
    topImg[cell] = getTop(cell);
}


// 重新计算所有格子的显示
void refreshAll() {
    for (int i = 0; i < n * n; i++)
        refresh(i);
}


// 修改图片内容 C
void change(int x, int r, int c) {
    int cell = r * n + c;
    bool old = hasImg(x, cell);

    setImg(x, r, c, !old);

    refresh(cell);
}


// 顺时针旋转
void rotateImg(int x) {

    vector<vector<int>> tmp(n, vector<int>(n));

    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            tmp[c][n - r - 1] = img[x][r*n+c];
        }
    }

    // 清除旧位置
    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            if (img[x][r*n+c])
                setImg(x,r,c,false);
        }
    }


    img[x].assign(n*n,0);

    for (int r=0;r<n;r++){
        for(int c=0;c<n;c++){
            img[x][r*n+c]=tmp[r][c];

            if(tmp[r][c])
                setImg(x,r,c,true);
        }
    }

    refreshAll();
}


// 左右镜像
void mirrorImg(int x){

    vector<vector<int>> tmp(n, vector<int>(n));

    for(int r=0;r<n;r++){
        for(int c=0;c<n;c++){
            tmp[r][n-c-1]=img[x][r*n+c];
        }
    }


    for(int r=0;r<n;r++)
        for(int c=0;c<n;c++)
            if(img[x][r*n+c])
                setImg(x,r,c,false);


    img[x].assign(n*n,0);


    for(int r=0;r<n;r++){
        for(int c=0;c<n;c++){
            img[x][r*n+c]=tmp[r][c];

            if(tmp[r][c])
                setImg(x,r,c,true);
        }
    }

    refreshAll();
}


// 移到最上面
void moveUp(int x){

    vector<int> tmp;

    for(int id:order)
        if(id!=x)
            tmp.push_back(id);

    tmp.push_back(x);

    order=tmp;

    refreshAll();
}


// 移到最下面
void moveDown(int x){

    vector<int> tmp;

    tmp.push_back(x);

    for(int id:order)
        if(id!=x)
            tmp.push_back(id);

    order=tmp;

    refreshAll();
}


int query(int x){

    int ans=0;

    for(int v:topImg)
        if(v==x)
            ans++;

    return ans;
}



int main(){

    ios::sync_with_stdio(false);
    cin.tie(nullptr);


    cin>>n>>k>>q;


    img.assign(k+1, vector<int>(n*n));

    words=(k+63)/64;

    maskCell.assign(n*n, vector<unsigned long long>(words));

    order.resize(k);

    for(int i=1;i<=k;i++){
        order[i-1]=i;

        string s;

        for(int r=0;r<n;r++){
            cin>>s;

            for(int c=0;c<n;c++){
                if(s[c]=='#'){
                    img[i][r*n+c]=1;
                    setImg(i,r,c,true);
                }
            }
        }
    }


    topImg.assign(n*n,0);

    refreshAll();



    while(q--){

        char op;
        cin>>op;


        if(op=='R'){

            int x;
            cin>>x;
            rotateImg(x);

        }else if(op=='M'){

            int x;
            cin>>x;
            mirrorImg(x);

        }else if(op=='C'){

            int x,r,c;
            cin>>x>>r>>c;

            r--;
            c--;

            change(x,r,c);

        }else if(op=='U'){

            int x;
            cin>>x;

            moveUp(x);

        }else if(op=='D'){

            int x;
            cin>>x;

            moveDown(x);

        }else if(op=='Q'){

            int x;
            cin>>x;

            cout<<query(x)<<"\n";
        }
    }


    // 最终视图
    for(int r=0;r<n;r++){

        for(int c=0;c<n;c++){

            int v=topImg[r*n+c];

            if(v==0)
                cout<<'.';
            else
                cout<<v;
        }

        cout<<"\n";
    }


    return 0;
}