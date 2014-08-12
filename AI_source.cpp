#include "Threes.h"
#include <ctime>
#include <climits>
#include <cmath>

#ifdef _WIN32
#include <conio.h>


#elif defined(__linux__)
#include <termios.h>
#include <unistd.h>
#include <stdio.h>


int getch(void){
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}
#endif
struct Infor{
    double score;
    int dir;
};
int dx[2]={-1,1};
int dy[2]={-1,1};
int cutTime=0,recusiveTime=0;
Game *game;
double max(double x,double y){return (x<y)?y:x;}

double abs(double x){
    return (x<0)?-x:x;
}

//double friction(

double Mono(Grid grid){
    double maxx=0;
    double maxy=0;
    int tmp;
    //ud
    for(int x=0;x<4;x++){
        double ans=0,reans=0;
        int t=grid(0,x);
        int now=0,next=1;
        double nowv=(grid(now,x)==0)?0:log2(grid(now,x)),nextv;
        while(next<4){
            while(grid(next,x)==0&&next<3)next++;
            double nextv=(grid(next,x)==0)?0:log2(grid(next,x));
            if(nextv>nowv)ans+=nextv-nowv;
            else reans+=nowv-nextv;
            now=next;
            nowv=nextv;
            next++;
        }
        maxx=max(maxx,max(ans,reans));
    }
    //rl
    for(int y=0;y<4;y++){
        double ans=0,reans=0;
        int t=grid(y,0);
        int now=0,next=1;
        double nowv=(grid(y,now)==0)?0:log2(grid(y,now));
        while(next<4){
            while(grid(y,next)==0&&next<3)next++;
            double nextv=(grid(y,next)==0)?0:log2(grid(y,next));
            if(nextv>nowv)ans+=nextv-nowv;
            else reans+=nowv-nextv;
            now=next;
            nowv=nextv;
            next++;
        }
        maxy=max(maxy,max(ans,reans));
    }


    return maxx+maxy;
}

double f(double a,double b){
    if(a==0||b==0)return 0;
    if(a==1&&b==1)return 2;
    if(a==2&b==2)return 2;
    if(a+b==3)return 0;
    if(a<=2||b<=2){
        double t;
        if(a<=2)t=a;
        if(b<=2)t=b;
        return 1+log2(t/3);
    }else{
        return abs(log2(b/3)-log2(a/3));
    }

}

double Smooth(Grid grid){
    double ans=0;
    //int g[16]={};
    //for(int i=0;i<15;i++)g[i]=(grid[i]==0)?-50:grid[i];
    for(int i=0;i<16;i++){
        if(i+1<16)
            //ans+=abs(grid[i]-grid[i+1]);
            ans+=f(grid[i],grid[i+1]);
        if(i+4<16)
            //ans+=abs(grid[i]-grid[i+4]);
            ans+=f(grid[i],grid[i+4]);
    }
    return ans;
}

double diff(Grid grid){
    int one,two;
    one=two=0;
    for(int i=0;i<16;i++){
        if(grid[i]==1)one++;
        if(grid[i]==2)two++;
    }
    return abs(one-two);
}

bool InBound(int x){return x>=0&&x<4;}
double Eval(Grid grid,int &mt,int &st,int &tt){
    //double t=(grid.getEmptyBlkNo()==0)?0:log2(grid.getEmptyBlkNo());
    /*
    double m=Mono(grid);
    double s=Smooth(grid);
    double d=diff(grid);
    double e=grid.getEmptyBlkNo();
    return m-s-d+5*e;*/
    int e=0;
    double score=0;
    double maxtile=0,sectile=0,thrtile=0;
    int maxX,maxY,secX,secY,thrX,thrY;
    for(int i=0;i<GRID_LENGTH;i++)
        for(int j=0;j<GRID_LENGTH;j++)
            if(maxtile<grid(i,j)){
                thrtile=sectile;
                thrX=secX;thrY=secY;
                sectile=maxtile;
                secX=maxX;secY=maxY;
                maxtile=grid(i,j);
                maxX=i;maxY=j;
            }else if(sectile<grid(i,j)){
                thrtile=sectile;
                thrX=secX;thrY=secY;
                sectile=grid(i,j);
                secX=i;secY=j;
            }else if(thrtile<grid(i,j)){
                thrtile=sectile;
                thrX=i;thrY=j;
            }
    //if(abs(secX-maxX)+abs(secY-maxY)<=1)score+=0.8;
    //if(secX==0||secX==GRID_LENGTH-1||secY==0||secY==GRID_LENGTH-1)score+=3.8;
    //if((thrX==0||thrX==GRID_LENGTH-1||thrY==0||thrY==GRID_LENGTH-1)&&abs(secX-thrX)+abs(secY-thrY)<=1)score+=2;
    //if((/*maxX==0||*/maxX==GRID_LENGTH-1)&&(/*maxY==0||*/maxY==GRID_LENGTH-1)) score+=10.8;
    //else if(maxX==0||maxX==GRID_LENGTH-1||maxY==0||maxY==GRID_LENGTH-1)score+=10.6;
    for(int i=0;i<GRID_LENGTH;i++){
        for(int j=0;j<GRID_LENGTH;j++){
            if(grid(i,j)==EMPTY){
                score+=5.2;
                continue;
            }
            if(InBound(i+1)){
                if(grid.canMerge(grid(i+1,j),grid(i,j)))score+=3.2;
                if(grid(i,j)==grid(i+1,j)*2||grid(i,j)*2==grid(i+1,j))score+=1.1;
                if(grid(i,j)<=2&&grid(i,j)==grid(i+1,j))score-=2.7;
                if(grid(i+1,j))score-=1.5*(abs(log2(grid(i,j)/3.0f)-log2(grid(i+1,j)/3.0f)));
            }
            if(InBound(j+1)){
                if(grid.canMerge(grid(i,j+1),grid(i,j)))score+=3.2;
                if(grid(i,j)==grid(i,j+1)*2||grid(i,j)*2==grid(i,j+1))score+=1.1;
                if(grid(i,j)<=2&&grid(i,j+1)==grid(i,j))score-=2.7;
                if(grid(i,j+1))score-=1.5*(abs(log2(grid(i,j)/3.0f)-log2(grid(1,j+1)/3.0f)));
            }
            if(i==0&&grid(i+1,j)>grid(i,j))score-=0.6;
            if(i==GRID_LENGTH-1&&grid(i-1,j)>grid(i,j))score-=0.6;
            if(j==0&&grid(i,j+1)>grid(i,j))score-=0.6;
            if(j==GRID_LENGTH-1&&grid(i,j-1)>grid(i,j))score-=0.6;
            if(InBound(i-1)&&InBound(i+1)){
                if(grid(i-1,j)>grid(i,j)&&grid(i+1,j)>grid(i,j))
                    score-=2.5;
            }
            if(InBound(j-1)&&InBound(j+1)){
                if(grid(i,j-1)>grid(i,j)&&grid(i,j+1)>grid(i,j))
                    score-=2.5;
            }

        }
    }
    mt=maxtile;
    st=sectile;
    tt=thrtile;
    return score+Mono(grid);
    //return grid.getScore();
}
double Eval2(Grid grid,int a,int b,int c){
    double score=0;
    for(int i=0;i<GRID_LENGTH;i++){
        for(int j=0;j<GRID_LENGTH;j++){
            if(grid(i,j)==EMPTY){
                score+=3;
                continue;
            }
            if(InBound(i+1)){
                if(grid.canMerge(grid(i+1,j),grid(i,j)))score+=2;
                if(grid(i,j)==grid(i+1,j)*2||grid(i,j)*2==grid(i+1,j))score+=1;
                //if(grid(i+1,j))score-=1.5*(abs(log2(grid(i,j)/3.0f)-log2(grid(i+1,j)/3.0f)));
            }
            if(InBound(j+1)){
                if(grid.canMerge(grid(i,j+1),grid(i,j)))score+=2;
                if(grid(i,j)==grid(i,j+1)*2||grid(i,j)*2==grid(i,j+1))score+=1;
                //if(grid(i,j+1))score-=1.5*(abs(log2(grid(i,j)/3.0f)-log2(grid(1,j+1)/3.0f)));
            }
            if(i==0&&grid(i+1,j)>grid(i,j))score-=1;
            if(i==GRID_LENGTH-1&&grid(i-1,j)>grid(i,j))score-=1;
            if(j==0&&grid(i,j+1)>grid(i,j))score-=1;
            if(j==GRID_LENGTH-1&&grid(i,j-1)>grid(i,j))score-=1;
            if(InBound(i-1)&&InBound(i+1)){
                if(grid(i-1,j)>grid(i,j)&&grid(i+1,j)>grid(i,j))
                    score-=1;
            }
            if(InBound(j-1)&&InBound(j+1)){
                if(grid(i,j-1)>grid(i,j)&&grid(i,j+1)>grid(i,j))
                    score-=1;
            }

        }
    }
    return score;
}

int deep=4;
Infor AISearch(int depth,double alpha,double beta,bool playerTurn,Grid grid){
    int a,b,c;
    recusiveTime++;
    double bestScore;
    int bestDir=-1;
    Infor res;
    if(depth<=0){
        res.dir=-1;
        res.score=Eval(grid,a,b,c);
        return res;
    }
    if(playerTurn){
        bestScore=alpha;
        bool move=false;
        for(int i=0;i<4;i++){
            Infor childRes;
            Grid newGrid;
            newGrid.copy(grid);
            if(newGrid.shift((dir_e)i)){
                move=true;
                childRes=AISearch(depth-1,bestScore,beta,false,newGrid);
                //if(i<=1)childRes.score/=3;
                if(childRes.score>bestScore){
                    bestScore=childRes.score;
                    bestDir=i;
                }
                if(bestScore>=beta){
                    cutTime++;
                    res.dir=bestDir;
                    res.score=beta;
                    return res;
                }
            }
        }
        if(!move)bestScore=-10000;
        res.dir=bestDir;
        res.score=bestScore;
        return res;
    }else{
        bestScore=beta;


        int n=grid.getSlotNo();
        for(int minsi=0;minsi<n;minsi++){
            int tile=1;
            int tileMax=3;
            char t=game->getHint();
            if(t!='+')tileMax=tile=t-'0';
            else tileMax=tile=6;
            Infor childRes;
/*
            int minsi=-1;
            double minn= INT_MAX;
            for(int i=0;i<n;i++){
                Grid newGrid;
                newGrid.copy(grid);
                newGrid.setSlot(i,tile);
                double evalue=Eval(newGrid,a,b,c);
                if(minn>evalue){
                    minn=evalue;
                    minsi=i;
                }
            }
*/
            Grid newGrid;
            newGrid.copy(grid);
            newGrid.setSlot(minsi,tile);
            childRes=AISearch(depth,alpha,bestScore,true,newGrid);
            if(childRes.score<bestScore){
                bestScore=childRes.score;
            }
            if(bestScore<=alpha){
                cutTime++;
                res.dir=-1;
                res.score=alpha;
                return res;
            }


        //}
        }
        res.dir=bestDir;
        res.score=bestScore;
        return res;
    }

}
int rmove;
int dMoveNewCard=2;
int dMove=1;
double AIESearch(int depth,Grid grid){
    int a,b,c;
    if(depth==dMoveNewCard+dMove){
        return Eval2(grid,a,b,c);
    }
    int bestMove=-1;
    double bestScore=-10000;


    for(int move=0;move<4;move++){
        double all=0;
        int count=0;
        Grid newGrid;
        newGrid.copy(grid);
        char next=game->getHint();
        if(newGrid.shift((dir_e)move)){
            if(depth<=dMoveNewCard){
                for(int i=0;i<newGrid.getSlotNo();i++){
                    if(depth==0){
                        if(next!='+'){
                            Grid ranGrid;
                            ranGrid.copy(newGrid);
                            int slot=next-'0';
                            ranGrid.setSlot(i,slot);
                            all+=AIESearch(depth+1,ranGrid);
                            count++;
                        }else{
                            int maxtile=grid.getMaxTile()/8;
                            for(int j=6;j<=maxtile;j*=2){
                                Grid ranGrid;
                                ranGrid.copy(newGrid);
                                ranGrid.setSlot(i,j);
                                all+=AIESearch(depth+1,ranGrid);
                                count++;
                            }
                        }
                    }else{
                        for(int j=1;j<=3;j++){
                            Grid ranGrid;
                            ranGrid.copy(newGrid);
                            ranGrid.setSlot(i,j);
                            all+=AIESearch(depth+1,ranGrid);
                            count++;
                        }
                    }

                }
            }else{
                all+=AIESearch(depth+1,newGrid);
                count++;
            }

            double expectedScore=all/count;
            if(bestMove==-1||expectedScore>bestScore){
                bestScore=expectedScore;
                bestMove=move;
            }
        }
    }


    if(depth==0)
        rmove=bestMove;
    return bestScore;
}

Infor IterativeDeep(Grid grid){
    Infor best=AISearch(deep,INT_MIN,INT_MAX,true,grid);
    return best;
}

const char* dirToStr(dir_e dir){
    if(dir == LEFT)   return "LEFT   ";
    if(dir == DOWN)   return "DOWN   ";
    if(dir == RIGHT)  return "RIGHT  ";
    if(dir == UP)     return "UP     ";
    return "INVALID";
}
int main(){
    Game aIGame;
    game = &aIGame;
    int score;
    int n=100;
    for(int i=0;i<n;i++){
        //srand(time(NULL));
        while(!aIGame.isGameOver(score)){

            int a,b,c;
            cutTime=recusiveTime=0;
            Grid grid;
            aIGame.getCurrentGrid(grid);
            //int n=grid.getSlotNo();
            //int index = grid.getSlotIndex(0);
            Eval(grid,a,b,c);
            //gotoXY(20,0);
/*
            Infor best=IterativeDeep(grid);
            if(best.dir==-1)best.dir=0;
            aIGame.insertDirection((dir_e)best.dir);
*/

            rmove=-1;

            AIESearch(0,grid);
            if(rmove==-1)rmove=0;
            aIGame.insertDirection((dir_e)rmove);

//            std::cout<<dirToStr((dir_e)best.dir)<<"       "<<best.score<<std::endl;
            //std::cout<<"CutTime"<<cutTime<<"       "<<"Recur"<<recusiveTime<<std::endl;
            //std::cout<<"1st:"<<a<<" 2nd"<<b<<" 3rd"<<c<<"               "<<std::endl;
            //std::cout<<"Now:"<<i<<std::endl;
            //aIGame.printGrid(5,4);
            //gotoXY(0,0);
            //Sleep(300000);

        }
        if(i<n-1)aIGame.reset();
    }
    //gotoXY(0,10);



}

