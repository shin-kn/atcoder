#include "library/standard.cpp"

ull solver(Array<ull>& arr,ull N){
    SegmentTree<ull> segtree(N+1,0ull,MaxFunc<ull>());
    range(i,v,arr){
        ull prev=segtree.Eval(0,v);
        segtree.Set(v,prev+1);
    }
    return segtree.Eval(0,N);

}


void solve(){
    ull N;
    cin>>N;
    Array<ull> P;
    rep(i,N) cin>>P[i];
    auto oper=[](Set<ull,2> val1,Set<ull,2> val2)->Set<ull,2>{
        if(val1[0]>val2[0]) return val1;
        return val2;
    };
    SegmentTree<Set<ull,2>> segtree(N,Set<ull,2>(0,0),oper);

    rep(i,N){
        segtree.Set(i,Set<ull,2>(P[i],i));
    }
    ull range=N-1;
    ull x_count=0;
    Array<bool> x_used(N,false);


    while(true){
        auto maxval=segtree.Eval(0,range);
        x_used[maxval[1]]=true;
        ++x_count;
        range=maxval[1];
        if(range==0) break;
        --range;
    }

    Array<ull> newarr;
    rep(i,N){
        if(!x_used[i]) newarr.Push(P[i]);
    }
    ull y_count=solver(newarr,N);
    cout<<x_count+y_count<<endl;



    

}