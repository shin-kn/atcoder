#include "library/standard.cpp"


ld get_angle(ld x,ld y){
    assert(x!=0.0 or y!=0.0);
    ld len=std::sqrt(x*x+y*y);
    ld angle=std::acos(x/len)/PI<ld>*ld(180.0);
    if(y<0 ){
        angle=ld(360.0)-angle;
    }
    return angle;


    

}



void solve(){
    ull N;
    cin>>N;
    Array<ld> X,Y;
    rep(i,N){
        cin>>X[i];
        cin>>Y[i];
    }
    Array<AVLTree<ld,ull>> arr;
    rep(i,N){
        rep(j,N){
            if(i==j) continue;
            arr[i].Push(get_angle(X[j]-X[i],Y[j]-Y[i]),j);
        }
    }

    auto abs=[&](ld n)->ld{
        if(n<=-360.0){
            n+=360.0;
        }
        if(n>=360.0){
            n-=360.0;
        }
        return Abs(n);
    };

    ld val=360.0;
    rep(i,N){
        arr[i].Geq(ld(0.0));
        ld zero_angle=arr[i].Index();
        arr[i].Leq(ld(360.0));
        ld full_angle=arr[i].Index();
        rep(j,N){
            if(i==j) continue;
            ld angle=get_angle(X[j]-X[i],Y[j]-Y[i]);
            ld wanted=angle+(ld)(180.0);
            if(wanted>=ld(360.0)) wanted-=(ld)360.0;
            
            UpdateMin(val,abs((wanted-zero_angle)));

            UpdateMin(val,abs((wanted-full_angle)));
            
            arr[i].Leq(wanted);
            UpdateMin(val,abs((wanted-arr[i].Index())));

            arr[i].Geq(wanted);
            UpdateMin(val,abs((wanted-arr[i].Index())));


        }
    }
    PrintDouble(ld(180.0)-val);




}