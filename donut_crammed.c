#include "donut.h"

void pt(T q,int A,float t,V l,Plane p){char*S=malloc(A*A);memset(S,32,A*A);float c=cos(t),s=sin(t);
float m[3][3]={{c,0,s},{s*s,c,-c*s},{-c*s,s,c*c}};for(float a=0;a<6.28;a+=0.0314){for(float b=0;b<6.28;b+=0.0314){
V d=rc(p,pj(p,am(m,tp(q,a,b))));int x=(A/2.0+d.x);int y=(A/2.0-d.y);float u=gb(q,a,b,m,l);char w=b2c(u);
int g=index_of(C,S[y*A+x]);if(g==-1||round(u*11.0)>g) S[y*A+x]=w;}}printf("\e[1;1H\e[2J");for(int r=0;r<A;r++){
putchar('|');for(int l=0;l<A;l++){putchar(S[r*A+l]);}printf("|\n");}}main(){for(float w=0;;w+=0.314){pt((T){(V){0,0,
3},15,7},58,w,(V){0,3,1},(Plane){(V){0},(V){0,0,1}});usleep(50000);}}