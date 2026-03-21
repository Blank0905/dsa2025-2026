void merge(int* initlist, int* mergelist, const int l, const int m, const int n) {
    for(int i1 = l, iresult = l, i2 = m+1;i1 <= m && i2 <= n;iresult ++) {
        if(initlist[i1] <= initlist[i2]) {
            mergelist[iresult] = initlist[i1];
            i1++;
        }
        else {
            mergelist[iresult] = initlist[i2];
            i2++;
        }
        copy(initlist+i1, initlist+m+1, mergelist+iresult);
        copy(initlist + i2, initlist + n + 1, mergelist + iresult);
    }
}

void merge_pass(int* initlist, int* resultlist, const int n, const int s) {
    int i;
    for (i = 1;i <= n-2*s+1;i+=2*s) {
        merge(initlist, resultlist, i, i+s-1, i+2*s-1);
        
    }
    if((i+s-1)<n){
            merge(initlist, resultlist, i, i+s-1, n);
        }
        else {
            copy(initlist+i, initlist+n+1, resultlist+i);
        }
    
}

void merge_sort(int* arr, const int n) {
    int* templist = new int[n+1];
    for (int l = 1; l< n;l*=2) {
        merge_pass(arr, templist, n, l);
        l*=2;
        merge_pass(templist, arr, n, l);
    }
    delete[] templist;
}