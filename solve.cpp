#include "library/standard.cpp"

void solve() {
	auto avlarray = AVLArray(Type<ull>, Type<ull>, LTOp<ull>, LTOp<ull>);
	avlarray[3] = 2;
	avlarray[1] = 2;
	avlarray[2] = 10;
	Print(avlarray.Min());
}

/*
void solve(){
    ll n;
    ll q;
    cin>>n>>q;
    LazySegmentAVLTree<ll,ll,ll>
tree(AddFunc<ll>(),AddFunc<ll>(),AddFunc<ll>()); rep(i,n){ ll val; cin>>val;
  tree.Push(i,val);
    }
  rep(i,q){
  ll com,x,y;
  cin>>com>>x>>y;
  if(com==0){
      tree.Action(y,x,x);
  }else{
      Print(tree.Eval(x,y-1));
  }
    }

}*/
/*
void solve_3(){
    ll N;
    ll Q;
    cin>>N>>Q;
    Array<ll> L,R;
    AVLTree<ll,ll> tree;
    tree.Push(N,N);
    if(N!=1) tree.Push(1,1);

    rep(i,Q){
  cin>>L[i]>>R[i];
  if(!tree.Find(L[i])) tree.Push(L[i],L[i]);
  if(!tree.Find(R[i]+1)) tree.Push(R[i]+1,R[i]+1);
    }
    Array<ll> indexarray=tree.IndexArray();






    auto add=[&](Set<ll,2> val1,Set<ll,2> val2)->Set<ll,2>{
  return Set<ll,2>(val1[0]+val2[0],val1[1]+val2[1]);
    };
    auto func=[&](ll action,Set<ll,2> value)->Set<ll,2>{
  if(action==0){
      return value;
  }
  value[0]=value[1];
  return value;
    };
    LazySegmentAVLTree<ll,Set<ll,2>,ll> avltree(add,func,MaxFunc<ll>());


    range(i,v,indexarray){
  if(i==indexarray.Length-1) break;
  avltree.Push(v,Set<ll,2>(0,indexarray[i+1]-indexarray[i]));
    }
    rep(i,Q){
  avltree.Action(1,L[i],R[i]);
  Print(N-avltree.Eval(1,N)[0]);
    }





}
void solve_2(){
    ll N;
    ll Q;
    cin>>N;
    cin>>Q;

    LazySegmentAVLTree<Set<ll,2>,ll,ll>
avltree(MaxFunc<ll>(),AddFunc<ll>(),AddFunc<ll>());

    Array<ll> A;
    Array<ll> B;

    rep(i,N) cin>>A[i];
    rep(i,N) cin>>B[i];

    rep(i,N){
  avltree.Push(Set<ll,2>(B[i],i),B[i]);
    }


    rep(i,N){
  avltree.Action(A[i],Set<ll,2>(0,0),Set<ll,2>(B[i],i));
    }

    rep(iiii,Q){
  ll q;
  ll ith;
  ll x;
  cin>>q>>ith>>x;
  --ith;
  if(q==1){//change A_i
      avltree.Action(-1*A[ith],Set<ll,2>(0,0),Set<ll,2>(B[ith],ith));
      A[ith]=x;
      avltree.Action(A[ith],Set<ll,2>(0,0),Set<ll,2>(B[ith],ith));
  }else{
      avltree.Action(-1*A[ith],Set<ll,2>(0,0),Set<ll,2>(B[ith],ith));
      avltree.Find(Set<ll,2>(B[ith],ith));
      avltree.Delete();
      B[ith]=x;
      ll base=0;
      if(avltree.Geq(Set<ll,2>(B[ith],ith))){
    Set<ll,2> index=avltree.Index();
    ll value=avltree.Eval(index,index);
    base=value-B[index[1]];
      }
      avltree.Push(Set<ll,2>(B[ith],ith),base+B[ith]);
      avltree.Action(A[ith],Set<ll,2>(0,0),Set<ll,2>(B[ith],ith));

  }
  cout<<avltree.Eval(Set<ll,2>(0,0),Set<ll,2>(Infty<ll>,0))<<endl;


    }






}
*/
