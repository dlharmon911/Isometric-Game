#include "main.h"

AstarPathFinderClass *astar;

/* A* class */ 
 
AstarPathFinderClass::AstarPathFinderClass(MAP_CLASS &map)
{
    Stack = (StackClass*)calloc(1,sizeof(StackClass));
    isPath = false;
    OpenNode = NULL;
    ClosedNode = NULL;
    PathNode = NULL;

    InitAstarTileMap(map);
}



/* deconstructor for Asterclass */ 
 

AstarPathFinderClass::~AstarPathFinderClass()
{
    FreeNodes();
    free(Stack);
    if ( TileMap )
        delete [] TileMap;
}



/* Init the astar class and setup the tiles */ 
 

void AstarPathFinderClass::InitAstarTileMap(MAP_CLASS &map)
{
    int i,index=0;

    Width = WORLD_X;
    Height = WORLD_Y;
    TotalSize = Height*Width;

    TileMap = new int[TotalSize];

    for (i=0; i<TotalSize; i++)
    {
        TileMap[index++]=map.tiles[i].level[3];
    }
}



/* If we did any changes to the map then we would have to fix the astar class */ 
 

void AstarPathFinderClass::RedoAstarTileMap(MAP_CLASS &map)
{
    int i,index=0;

    for (i=0; i<TotalSize; i++)
    {
        TileMap[index++]=map.tiles[i].level[3];
    }
}



/* returns true if there is a path from sx,sy to dx,dy */ 
 

bool AstarPathFinderClass::NewPath(int sx,int sy, int dx,int dy)
{
    if ( FreeTile(dx,dy)&&FreeTile(sx,sy) && (TileNum(sx,sy)!=TileNum(dx,dy)) )
    {
        isPath=true;


/* clear NodeClass lists */ 
 

        FreeNodes();

        FindPath(sx,sy,dx,dy);
        return (isPath);
    }

    return (isPath=false);
}



/* returns true if we reached the goal */ 
 

bool AstarPathFinderClass::ReachedGoal(void)
{


/* the NodeClass entries this looks a little bit strange */ 
 

    if ( !isPath ) return true;



/* but prevents illegal calls of ::PathNextNode() or ::PathGet*() */ 
 

    if ( PathNode->Parent != NULL )
        return false;

    return true;
}



/* returns the tile number from the supplied x,y coordinates */ 
 

int AstarPathFinderClass::TileNum( int x , int y )
{
    return map.getPos(x,y);
}



/* returns if the tile is open or not */ 
 

bool AstarPathFinderClass::FreeTile(int x, int y)
{
    if ( TileMap[ TileNum( x , y ) ] != 1 ) return true;
    
    return false;
}



/* free any nodes in memory */ 
 

void AstarPathFinderClass::FreeNodes(void)
{
    NodeClass *NodeClass, *OldNodeClass;

    if ( OpenNode != NULL )
    {
        NodeClass = OpenNode->NextNode;

        while ( NodeClass != NULL )
        {
            OldNodeClass = NodeClass;
            NodeClass = NodeClass->NextNode;
            free(OldNodeClass);
        }
    }

    if ( ClosedNode != NULL )
    {
        NodeClass = ClosedNode->NextNode;

        while ( NodeClass != NULL )
        {
            OldNodeClass = NodeClass;
            NodeClass = NodeClass->NextNode;
            free(OldNodeClass);
        }
    }
}



/* set the current node to the next node */ 
 

void AstarPathFinderClass::PathNextNode(void)
{
    PathNode=PathNode->Parent;
}



/* return the x value of the node */ 
 

int AstarPathFinderClass::NodeGetX(void)
{
    return PathNode->x;
}



/* returns the y value of the node */ 
 

int AstarPathFinderClass::NodeGetY(void)
{
    return PathNode->y;
}




/* find a path from sx,sy to dx,dy */ 
 

void AstarPathFinderClass::FindPath(int sx, int sy, int dx, int dy)
{
    NodeClass *Node, *BestNode;
    int TileNumDest;

    isPath=true;
    TileNumDest = TileNum(sx, sy);

    OpenNode=( NodeClass* )calloc(1,sizeof( NodeClass ));
    ClosedNode=( NodeClass* )calloc(1,sizeof( NodeClass ));

    Node=( NodeClass* )calloc(1,sizeof( NodeClass ));
    Node->g = 0;
    Node->h = distance((dx-sx),(dy-sy));
    Node->f = Node->g+Node->h;
    Node->NodeNum = TileNum(dx, dy);
    Node->x = dx;
    Node->y = dy;



/* make OpenNode List point to first NodeClass */ 
 

    OpenNode->NextNode=Node;
    for (;;)
    {
        BestNode=ReturnBestNode();

        if (BestNode==NULL) break;



/* if we've found the end, break and finish */ 
 

        if (BestNode->NodeNum == TileNumDest)
            break;

        GenerateSuccessors(BestNode,sx,sy);
    }

    PathNode = BestNode;
}



/* find out which node is the best and return it */ 
 

NodeClass *AstarPathFinderClass::ReturnBestNode(void)
{
   NodeClass *tmp;

   if ( OpenNode->NextNode == NULL )
   {
        isPath=false;
        tmp=NULL;
        return tmp;
   }



/* Pick NodeClass with lowest f, in this case it's the first NodeClass in list because we sort the OpenNode list wrt lowest f. Call it BestNode. point to first NodeClass on OpenNode */ 
 

    tmp = OpenNode->NextNode;
    


/* Make OpenNode point to nextNodeClass or NULL. */ 
 

    OpenNode->NextNode = tmp->NextNode;



/* Next take BestNode (or temp in this case) and put it on ClosedNode */ 
 

    tmp->NextNode = ClosedNode->NextNode;
    ClosedNode->NextNode = tmp;

   return(tmp);
}



/* I commented out four of the directions because I wanted my ball to move only vertically and horizontally. You can use the other four directions if you want */ 
 

void AstarPathFinderClass::GenerateSuccessors(NodeClass *BestNode, int dx, int dy)
{
    int x, y;

    if ( FreeTile(x=BestNode->x-64, y=BestNode->y-32) )
        GenerateSucc(BestNode,x,y,dx,dy);

    if ( FreeTile(x=BestNode->x+64, y=BestNode->y-32) )
        GenerateSucc(BestNode,x,y,dx,dy);

    if ( FreeTile(x=BestNode->x-64, y=BestNode->y+32) )
        GenerateSucc(BestNode,x,y,dx,dy);

    if ( FreeTile(x=BestNode->x+64, y=BestNode->y+32) )
        GenerateSucc(BestNode,x,y,dx,dy);
}



/* */ 
 

void AstarPathFinderClass::GenerateSucc(NodeClass *BestNode,int x, int y, int dx, int dy)
{
    int g, TileNumS, c = 0;
    NodeClass *Old, *Successor;



/* g(Successor)=g(BestNode)+cost of getting from BestNode to Successor */ 
 


    g = BestNode->g+1;


/* identification purposes */ 
 


    TileNumS = TileNum(x,y);



/* if equal to NULL then not in OpenNode list, else it returns the NodeClass in Old */ 
 

    if ( (Old=CheckOPEN(TileNumS)) != NULL ) 
    {
        for( c = 0; c < MAX_CHILDREN; c++)
        {


/* Add Old to the list of BestNode's Children (or Successors) */ 
 

            if( BestNode->Child[c] == NULL )            {
                break;
            }
        }
        BestNode->Child[c] = Old;



/* if our new g value is < Old's then reset Old's parent to point to BestNode */ 
 

        if ( g < Old->g )
        {
            Old->Parent = BestNode;
            Old->g = g;
            Old->f = g + Old->h;
        }
    }
    // if equal to NULL then not in OpenNode list, else it returns the NodeClass in Old
    else if ( (Old=CheckCLOSED(TileNumS)) != NULL ) 
    {
        for( c = 0; c< MAX_CHILDREN; c++)
        {


/* Add Old to the list of BestNode's Children (or Successors). */ 
 

            if ( BestNode->Child[c] == NULL )
                break;
        }
        
        BestNode->Child[c] = Old;



/* if our new g value is < Old's then reset Old's parent to point to BestNode */ 
 

        if ( g < Old->g )
        {
            Old->Parent = BestNode;
            Old->g = g;
            Old->f = g + Old->h;


/* Since we changed the g value of Old, we need to propagate this new value downwards, i.e. do a Depth-First traversal of the tree! */ 
 

            PropagateDown(Old);  
        }
    }
    else
    {
        Successor = ( NodeClass* )calloc(1,sizeof( NodeClass ));
        Successor->Parent = BestNode;
        Successor->g = g;
        Successor->h = distance((x-dx),(y-dy));
        Successor->f = g+Successor->h;
        Successor->x = x;
        Successor->y = y;
        Successor->NodeNum = TileNumS;



/* Insert Successor on OpenNode list wrt f */ 
 

        Insert(Successor);    
        for( c =0; c < MAX_CHILDREN; c++)
        {


/* Add Old to the list of BestNode's Children (or Successors). */ 
 

            if ( BestNode->Child[c] == NULL )
                break;
        }
        BestNode->Child[c] = Successor;
    }
}



/* */ 
 

NodeClass *AstarPathFinderClass::CheckOPEN(int tilenum)
{
    NodeClass *tmp;

    tmp = OpenNode->NextNode;
    while ( tmp != NULL )
    {
        if ( tmp->NodeNum == tilenum )
        {
            return (tmp);
        }
        else
        {
            tmp = tmp->NextNode;
        }
    }

    return(NULL);
}



/* */ 
 

NodeClass* AstarPathFinderClass::CheckCLOSED(int tilenum)
{
    NodeClass *tmp;

    tmp = ClosedNode->NextNode;

    while ( tmp != NULL )
    {
        if ( tmp->NodeNum == tilenum )
        {
            return(tmp);
        }
        else
        {
            tmp = tmp->NextNode;
        }
    }

    return(NULL);
}



/* */ 
 

void AstarPathFinderClass::Insert(NodeClass *Successor)
{
    NodeClass *tmp1, *tmp2;
    int f;

    if ( OpenNode->NextNode == NULL )
    {
        OpenNode->NextNode = Successor;
        return;
    }

    // insert into OpenNode successor wrt f
    f = Successor->f;
    tmp1 = OpenNode;
    tmp2 = OpenNode->NextNode;

    while ( (tmp2 != NULL) && (tmp2->f < f) )
    {
        tmp1 = tmp2;
        tmp2 = tmp2->NextNode;
    }

    Successor->NextNode = tmp2;
    tmp1->NextNode = Successor;
}



/* */ 
 

void AstarPathFinderClass::PropagateDown(NodeClass *Old)
{
    int c, g;
    NodeClass *Child, *Father;

    // alias.
    g = Old->g;

    for ( c = 0; c < MAX_CHILDREN; c++)
    {
        // create alias for faster access.
        if ( (Child=Old->Child[c]) == NULL )
            break;
        
        if ( g+1 < Child->g )
        {
            Child->g = g+1;
            Child->f = Child->g + Child->h;
            // reset parent to new path.
            Child->Parent = Old;


/* Now the Child's branch need to be checked out. Remember the new cost must be propagated down. */ 
 

            Push(Child);         
        }     
    }

    while ( Stack->NextStackPtr != NULL )
    {
        Father = Pop();
        for (c = 0; c<MAX_CHILDREN; c++)
        {
            // we may stop the propagation 2 ways: either
            if ( (Child=Father->Child[c]) == NULL )
                break;
            


/* there are no children, or that the g value of the child is equal or better than the cost we're propagating */ 
 

            if ( Father->g+1 < Child->g )
            {              
                Child->g = Father->g+1;
                Child->f = Child->g+Child->h;
                Child->Parent = Father;
                Push(Child);
            }
        }
    }
}



/* put a node on the stack */ 
 

void AstarPathFinderClass::Push(NodeClass *NodeClass)
{
    StackClass *tmp;

    tmp =( StackClass* )calloc(1,sizeof( StackClass ));
    tmp->NodePtr = NodeClass;
    tmp->NextStackPtr = Stack->NextStackPtr;
    Stack->NextStackPtr = tmp;
}



/* grab a node from the stack */ 
 

NodeClass *AstarPathFinderClass::Pop(void)
{
    NodeClass *tmp;
    StackClass *tmpSTK;

    tmpSTK = Stack->NextStackPtr;
    tmp = tmpSTK->NodePtr;

    Stack->NextStackPtr = tmpSTK->NextStackPtr;
    free(tmpSTK);

   return(tmp);
}
