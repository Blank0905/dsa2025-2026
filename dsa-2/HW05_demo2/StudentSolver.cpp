#ifndef STUDENTSOLVER_CPP
#define STUDENTSOLVER_CPP

#include "Maze.cpp"

void mypush(PathNode*& stackTop, int r, int c) { //stack push
    PathNode* node = new PathNode(r, c, stackTop);
    stackTop = node;
}

bool mypop(PathNode*& stackTop, int& r, int& c) { //stack pop
    if(!stackTop) {return false;}
    r = stackTop -> r;
    c = stackTop -> c;
    PathNode* temp = stackTop;
    stackTop = stackTop->next;
    delete temp;
    return true;
}

void enqueue(PathNode*& head, PathNode*& tail, int& r, int& c) {
    PathNode* node = new PathNode(r, c);
    if(!head) {
        head = tail = node;
    }
    else {
        tail -> next = node;
        tail = node;
    }
}

bool dequeue(PathNode*& head, PathNode*& tail, int& r, int& c) {
    if(!head) {return false;}
    r = head->r;
    c = head->c;
    PathNode* temp = head;
    head = head -> next;
    delete temp;
    return true;
}

bool solveByDFS(const Maze& maze, PathNode*& path) {
    int n = maze.getN();
    path = nullptr;
    //TODO 1 : Solve by DFS
    bool** visited = new bool*[n];
    int** parentR = new int *[n];
    int** parentC = new int *[n];
    for(int i=0; i<n; i++) {
        visited[i] = new bool[n](); //把每一格都設成false
        parentR[i] = new int[n];
        parentC[i] = new int[n];
        for(int j=0; j<n;j++) {
            parentR[i][j] = -1;
            parentC[i][j] = -1;
        }
    }

    PathNode* stackTop = nullptr;
    int sR = maze.getStartR();
    int sC = maze.getStartC();
    int eR = maze.getEndR();
    int eC = maze.getEndC();
    mypush(stackTop, sR, sC);

    int dr[4] = {1, -1, 0, 0};
    int dc[4] = {0, 0, 1, -1};
    bool found = false;
    
    while (stackTop) {
        int r;
        int c;
        mypop(stackTop, r, c);

        if(!visited[r][c]) {
            visited[r][c] = true;

            if(r == eR && c == eC) {
                found = true;
                break;
            }

            for(int i=0; i<4; i++) {
                int nr = r+dr[i];
                int nc = c+dc[i];

                if(nr>=0 && nr<n && nc>=0 && nc<n) {
                    if(maze.getGrid(nr, nc) == 0 && !visited[nr][nc]) {
                        parentR[nr][nc] = r;
                        parentC[nr][nc] = c;
                        mypush(stackTop, nr, nc);   
                    }
                }
            }
        }
    }

    if(found) {
        int currR = eR;
        int currC = eC;
        while(currR != sR || currC != sC) {
            PathNode* node = new PathNode(currR, currC, path);
            path = node;
            int tempR = parentR[currR][currC];
            int tempC = parentC[currR][currC];
            currR = tempR;
            currC = tempC;
        }
        PathNode* start = new PathNode(sR, sC, path);
        path = start;
    }
    while(stackTop) {
        int r, c;
        mypop(stackTop, r, c);
    }
    for(int i=0; i<n; i++) {
        delete[] visited[i];
        delete[] parentR[i];
        delete[] parentC[i];
    }

    delete[] visited;
    delete[] parentR;
    delete[] parentC;

    return found;
}

bool solveByBFS(const Maze& maze, PathNode*& path) {
    int n = maze.getN();
    path = nullptr;
    //TODO 2 : Solve by BFS
    bool** visited = new bool*[n];
    int** parentR = new int *[n];
    int** parentC = new int *[n];
    for(int i=0; i<n; i++) {
        visited[i] = new bool[n](); //把每一格都設成false
        parentR[i] = new int[n];
        parentC[i] = new int[n];
        for(int j=0; j<n;j++) {
            parentR[i][j] = -1;
            parentC[i][j] = -1;
        }
    }

    PathNode* tail = nullptr;
    PathNode* head = nullptr;
    
    int sR = maze.getStartR();
    int sC = maze.getStartC();
    int eR = maze.getEndR();
    int eC = maze.getEndC();

    int dr[4] = {1, -1, 0, 0};
    int dc[4] = {0, 0, 1, -1};
    bool found = false;
    enqueue(head, tail, sR, sC);

    while(head) {
        int r;
        int c;
        dequeue(head, tail, r, c);

        if(!visited[r][c]) {
            visited[r][c] = true;

            if(r == eR && c == eC) {
                found = true;
                break;
            }

            for(int i=0; i<4; i++) {
                int nr;
                int nc;
                nr = r + dr[i];
                nc = c + dc[i];
                if(nr >= 0 && nr < n && nc >= 0 && nc < n) {
                    if(maze.getGrid(nr, nc) == 0 && !visited[nr][nc] && parentR[nr][nc] == -1) {
                        parentR[nr][nc] = r;
                        parentC[nr][nc] = c;
                        enqueue(head, tail, nr, nc);
                    }
                }
            }
        }
    }

    if(found) {
        int currR = eR;
        int currC = eC;
        while(currR != sR || currC != sC) {
            mypush(path, currR, currC);
            int tempR = parentR[currR][currC];
            int tempC = parentC[currR][currC];
            currR = tempR;
            currC = tempC;
        }
        mypush(path, sR, sC);
    }

    while(head) {
        int r, c;
        dequeue(head, tail, r, c);
    }
    for(int i=0; i<n; i++) {
        delete[] visited[i];
        delete[] parentR[i];
        delete[] parentC[i];
    }

    delete[] visited;
    delete[] parentR;
    delete[] parentC;

    return found;
}

#endif
