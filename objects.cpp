#include "objects.h"



/*



int object_queue_findempty(int i)
{
    if(objects[i]==NULL) return i;
    for(i=OBJECTS_LAST;i>=0;i--) if(objects[i]==NULL) return i;
    return -1;
}
int object_queue_add(void* so, int type)
{
    int i=object_queue_findempty(object_next);
    if(i<0)
    {
        object_queue_free(so);//prevents items not queued from being leaked.
        object_next=-1;
        return -1;
    }else{
        objects[i]=so;
        types[i]=type;
        if(i==OBJECTS_MAX) object_next=-1;
        else object_next=i+1;
        return i;
    }
}
void object_queue_remove(int i,bool free)
{
    if(free) object_queue_free(i);
    objects[i]=NULL;//we need to free this
    types[i]=OT_NONE;
    object_next=i;
}
void object_queue_clear(bool free)
{
    for(int i=0;i<OBJECTS_MAX;i++) object_queue_remove(i,free);
    object_next=0;
}
int object_queue_count()
{
    int c=0;
    for(int i=0;i<OBJECTS_MAX;i++) if(objects[i]!=NULL) c++;
    return c;
}
void object_queue_step()
{
    for(int i=0;i<OBJECTS_MAX;i++) object_queue_step(i);
}
void object_queue_free(int i)
{
    if(objects[i]==NULL) return;
    object_queue_free(objects[i],types[i]);
}

int object_queue_add(void* so) {return object_queue_add(so,((SceneryObject*) so)->type);}

void object_queue_unloadimages(Drawing_Info* di)
{
    for(int i=0;i<OBJECTS_MAX;i++)
        if(objects[i]!=NULL){
            SceneryObject* so=(SceneryObject*) objects[i];
            if(so->ii!=NULL) so->ii->Unload();
        }
}
void object_queue_reloadimages(Drawing_Info* di)
{
    for(int i=0;i<OBJECTS_MAX;i++)
        if(objects[i]!=NULL){
            SceneryObject* so=(SceneryObject*) objects[i];
            if(so->ii!=NULL) so->ii->Reload(di);
        }
}

void object_queue_draw(Drawing_Info* di)
{
    for(int i=0;i<OBJECTS_MAX;i++)
        if(objects[i]!=NULL) 
            ((SceneryObject*) objects[i])->Draw(di);
}

void object_queue_step(int i)
{
    if(objects[i]==NULL) return;
    SceneryObject* so=(SceneryObject*) objects[i];
    if(!(so->visible)) return object_queue_remove(i,true);
    so->doStep(i);
    if(!(so->visible)) object_queue_remove(i,true);
}
int object_queue_owner(int i)
{
    int owner=i;
    if(types[i]==OT_PROJ){
        if(objects[i]!=NULL){
            int tmpowner=((Projectile*) objects[i])->object_owner;
            if(tmpowner>=0) owner=object_queue_owner(tmpowner);
        }
    }
    return owner;
}
int object_queue_assoc(int i, int j)
{
    i=object_queue_owner(i);
    j=object_queue_owner(j);
    
    NPCShip* ns=NULL;
    SceneryObject* so=NULL;
    Projectile* pj=NULL;
    if(types[i]==OT_PROJ) pj=(Projectile*) objects[i];
    if(types[j]==OT_PROJ) pj=(Projectile*) objects[j];
    if(pj!=NULL) if(pj->object_owner<-1) return pj->getUnownedView();
    
    if(types[i]==OT_NSHP)
    {
        
        ns=(NPCShip*) objects[i];
        so=(SceneryObject*) objects[j];
    }
    if(types[j]==OT_NSHP)
    {
        ns=(NPCShip*) objects[j];
        so=(SceneryObject*) objects[i];
    }
    if(ns!=NULL && so!=NULL) return ns->getAssoc(so);
    return NEUTRAL;
}
void object_queue_free(void* p,int type)
{
    SceneryObject* so=(SceneryObject*) p;
    delete so;//virtual destructors will get called as a fall-through.(BCPP GP p306)
    
    //if(p==NULL) return;
    //SceneryObject* so=(SceneryObject*) p;
    //if(type==OT_NONE) type=so->type;
    //switch(type)
    //{
    //    case OT_NSHP:
    //        delete ((NPCShip*) p);
    //        break;
    //    case OT_PSHP:
    //        delete ((PlayerShip*) p);
    //        break;
    //    case OT_SHIP:
    //        delete ((Ship*) p);
    //        break;
    //    case OT_PROJ:
    //        delete ((Projectile*) p);
    //        break;
    //    case OT_PHYS:
    //        delete ((PhysicalObject*) p);
    //        break;
    //    case OT_EXPL:
    //        delete ((Explosion*) p);
    //        break;
    //    default:
    //        delete ((SceneryObject*) p);
    //}
}

*/
