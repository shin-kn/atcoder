#include "library/standard.cpp"

void solve(){
    ull N;
    cin>>N;
    string s;
    cin>>s;

    Array<NMod> coef;
    coef[2]=2;

    rep_3(k,3,N+1){
        coef[k]=Factorial<NMod>(k);
        rep_3(l,2,k){
            coef[k]-=Factorial<NMod>(k-l+1)*coef[l];
        }
    }
   
    NMod pat=NMod(1);
    ull x_count=0;
    rep(i,s.length()){
        if(s[i]=='o'){
            if(i==0) continue;
            if(x_count==0){
                pat*=NMod(2);
            }else{
                pat*=coef[x_count+2];
            }
            x_count=0;
        }else{
            ++x_count;
        }
    }
    if(x_count!=0){
        pat=0;
    }
    if(s[0]=='x') pat=0;
    
    Print(pat);


    




}