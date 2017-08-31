//
//  Navigator.cpp
//  maps
//
//  Created by Tiffany Kyu on 3/10/17.
//  Copyright © 2017 Tiffany Kyu. All rights reserved.
//

#include "provided.h"
#include <string>
#include <vector>
#include <stack>
#include "support.h"
#include <iostream>

using namespace std;

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
    
private:
    struct Node //Node placed in open/closed list of A* algorithm
    {
        StreetSegment m_seg;
        Node* m_parent;
        double f,g,h;
        /*
        bool operator< (const Node &c1) //< overload of Node
        {
            return f > c1.f;
        }
         */
    };
    MapLoader ml;
    AttractionMapper am;
    SegmentMapper sm;
    
    ///Helper functions///
    bool bestRoute(string start, string end, stack<StreetSegment>& track) const;    //A* for best route
    bool isOnList(vector<Node*> lis, StreetSegment ss, int& val) const; //Is a segment on a list already?
    string directionTravel(GeoSegment& ins) const;   //Returns compass of driving
    string turnTravel(GeoSegment& ins1, GeoSegment& ins2) const;  //Turn left or right
};

NavigatorImpl::NavigatorImpl()
{}

NavigatorImpl::~NavigatorImpl()
{}

string NavigatorImpl::directionTravel(GeoSegment& ins) const    //Gets directions of travel
{
    double angle = angleOfLine(ins);
    
    if(angle >= 0 && angle <= 22.5)
        return "east";
    else if(angle > 22.5 && angle <= 67.5)
        return "northeast";
    else if(angle > 67.5 && angle <= 112.5)
        return "north";
    else if(angle > 112.5 && angle <= 157.5)
        return "northwest";
    else if(angle > 157.5 && angle <= 202.5)
        return "west";
    else if(angle > 202.5 && angle <= 247.5)
        return "southwest";
    else if (angle > 247.5 && angle <= 292.5)
        return "south";
    else if(angle > 292.5 && angle <= 337.5)
        return "southeast";
    else if(angle > 337.5 && angle <=360)
        return "east";
    
    return "";
}

string NavigatorImpl::turnTravel(GeoSegment& ins1, GeoSegment& ins2) const  //Which way am I turning?
{
    double angle = angleBetween2Lines(ins1, ins2);
    
    if( angle < 180)
        return "left";
    else
        return "right";
}


bool NavigatorImpl::isOnList(vector<Node*> lis, StreetSegment ss, int& val) const
{
    for(int i = 0; i < lis.size(); i++) //Goes through the vector provided
    {
        //If the provided segment matches something in list (name&GeoCoord)
        if(lis[i]->m_seg.streetName == ss.streetName && lis[i]->m_seg.segment.start == ss.segment.start && lis[i]->m_seg.segment.end == ss.segment.end)
        {
            val = i;    //Pass by its element number on list
            return true;
        }
    }
    
    return false;   //Go through list and not found
}

bool NavigatorImpl::bestRoute(string start, string end, stack<StreetSegment>& track) const
{
    vector<Node*> open, close;
    
    //Geocoords of attractions
    GeoCoord gc, gc2;
    am.getGeoCoord(start, gc);
    am.getGeoCoord(end, gc2);
    
    //Get streetsegs associated with each attraction
    vector<StreetSegment> check = sm.getSegments(gc);
    vector<StreetSegment> check2 = sm.getSegments(gc2);
    
    //Create Node for attractions
    Node* begin = new Node;
    Node destin;
    begin->m_seg = check[0];
    begin->m_parent = nullptr;
    begin->f = 0;
    begin->g = 0;
    begin->h = 0;
    destin.m_seg = check2[0];
    
    open.push_back(begin);  //Push starting position onto open list
    Node* cur = nullptr;    //Used to point to Node of destination if you find it
    
    while(!open.empty())    //Until you go through entire open list or you find destination
    {
        int val, openRemoval = 0;
        
        if(isOnList(close,destin.m_seg,val))    //If the destination is on the closed list, found path
        {
            //cur = close[val];   //Point to destintion node
            break;
        }
        
        Node* lowestF = open[0];   //Find node with lowest F value in open list
        for(int i = 1; i < open.size(); i++)
        {
            if( open[i]->f < lowestF->f)
            {
                lowestF = open[i];
                openRemoval = i;
            }
        }
                 
        close.push_back(lowestF);  //Put lowest F in the closed list
        open.erase(open.begin() + openRemoval); //Remove from open list
        
        //All adjacent street segments because share same end and start points
        vector<StreetSegment> ss1 = sm.getSegments(lowestF->m_seg.segment.start);
        vector<StreetSegment> ss2 = sm.getSegments(lowestF->m_seg.segment.end);
        
        //Add 2 vectors together
        ss1.insert(ss1.end(), ss2.begin(), ss2.end());
        
        for(int j = 0; j < ss1.size(); j++) //Go through each adjacent segment
        {
            if(isOnList(close,ss1[j],val))  //If on closed list, ignore neighbor
                continue;
            
            if(!isOnList(open,ss1[j],val))  //If not on open list, create and add node to open list
            {
                Node* add = new Node;
                add->m_seg = ss1[j];
                add->m_parent = lowestF;
                add->g = add->m_parent->g + distanceEarthMiles(add->m_parent->m_seg.segment.end, add->m_seg.segment.end);
                /*
                if(distanceEarthMiles(add->m_parent->m_seg.segment.end, add->m_seg.segment.end) == 0)
                    add->g = add->m_parent->g + distanceEarthMiles(add->m_parent->m_seg.segment.end, add->m_seg.segment.start);
                else
                    add->g = add->m_parent->g + distanceEarthMiles(add->m_parent->m_seg.segment.end, add->m_seg.segment.end);
                 */
                add->h = distanceEarthMiles(add->m_seg.segment.end, gc2);
                add->f = add->g + add->h;
                
                open.push_back(add);
            }
            else    //On open list already
            {
                double newG;
                if(distanceEarthMiles(lowestF->m_seg.segment.end, ss1[j].segment.end) != 0)
                {
                    newG = lowestF -> g + distanceEarthMiles(lowestF->m_seg.segment.end, ss1[j].segment.end);
                }
                else
                    newG = lowestF -> g + distanceEarthMiles(lowestF->m_seg.segment.end, ss1[j].segment.start);
                if( newG < open[val]->g)    //Check to see if going to Node this way has lower F than the one it already has
                    open[val]->m_parent = lowestF;  //If so, change parent to this way
            }
            
        }
    }
    
    int val = 0;
    if(!isOnList(close,destin.m_seg,val) && open.empty())    //If empty at end, then no found route
    {
        for(int j = 0; j < close.size(); j++)
            delete close[j];
        return false;
    }
    
    //isOnList(close,destin.m_seg,val);
    cur = close[val];
    
    while(cur != nullptr)   //Follows pointers to parent nodes to find route to beginning
    {
        StreetSegment copy = cur->m_seg;    //Create copy of street segment
        track.push(copy);   //Add copy to stack
        cur = cur->m_parent;    //Follow to next parent
    }
    
    //Delete all allocated Nodes left in open and close list
    for(int i = 0; i < open.size(); i++)
        delete open[i];
    for(int j = 0; j < close.size(); j++)
        delete close[j];
    
    return true;    //Found route
}

bool NavigatorImpl::loadMapData(string mapFile)
{
    if (! ml.load(mapFile)) //Load map file
        return false;   //If fail, return false
    
    //Initialize attraction mapper and segment mapper
    am.init(ml);
    sm.init(ml);
    
    return true;
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
    GeoCoord gc,gc2;
    if(!(am.getGeoCoord(start, gc)))    //Can't find start attraction
        return NAV_BAD_SOURCE;
    
    if( !(am.getGeoCoord(end, gc2)))    //Can't find end attraction
        return NAV_BAD_DESTINATION;
    
    if(!directions.empty()) //If directions isn't passed in empty, erase everything insie
        directions.erase(directions.begin(),directions.end());
    
    if(start == end)    //If it's the same location, just reutrn an empty directions vector and success
        return NAV_SUCCESS;
    
    stack<StreetSegment> rawDir;
    
    if( !(bestRoute(start, end, rawDir)))   //If return false, then no route found
        return NAV_NO_ROUTE;
    
    StreetSegment lastName = rawDir.top();  //Holds info of previous streetseg
    GeoSegment preWay, thisWay;
    preWay.end = gc;
    bool justTurn = false;
    
    while(!rawDir.empty())  //Go through vector of segments
    {
        StreetSegment copy = rawDir.top();  //Holds info of current streetseg looking at
        
        NavSegment add; //Add new navSegment to directions
        //directions.push_back(*new NavSegment);
        directions.push_back(add);
        int curEl = directions.size() - 1;  //Element number you're adding to
        
        if(copy.streetName == lastName.streetName)  //Proceeding on same street
        {
            if(!justTurn)   //If you didn't just turn, get all info for start and end GeoCoord in direction of travel
            {
                rawDir.pop();
                thisWay.start = preWay.end;
                
                if(rawDir.empty())  //Looking at last segment, so ending coordinate must be here
                    thisWay.end = gc2;
                else
                {
                    if(copy.segment.end == rawDir.top().segment.start || copy.segment.start == rawDir.top().segment.start)
                        thisWay.end = rawDir.top().segment.start;
                    else if(copy.segment.end == rawDir.top().segment.end || copy.segment.start== rawDir.top().segment.end)
                        thisWay.end = rawDir.top().segment.end;
                }
                preWay = thisWay;
                lastName = copy;
            }
            
            //If you just turned and next direction isn't a turn, then already proceesed GeoSegment info of street into thisWay, so just add that info it
            
            //Add all necessary info to NavSegment
            directions[curEl].m_command = NavSegment::PROCEED;
            directions[curEl].m_streetName = copy.streetName;
            directions[curEl].m_distance = distanceEarthMiles(thisWay.start, thisWay.end);
            directions[curEl].m_direction = directionTravel(thisWay);
            directions[curEl].m_geoSegment = thisWay;
            
            if(justTurn)
                justTurn = false;
        }
        else    //Current street different from last one, so need to turn
        {
            //Get all info for start and end GeoCoord in direction of travel for segment you're turning onto
            thisWay.start = preWay.end;
            
            if(rawDir.size() == 1)  //Turning onto last segment, so, dont pop off the top otherwise can't process that last street
                thisWay.end = gc2;
            else
            {
                rawDir.pop();
                if(copy.segment.end == rawDir.top().segment.start || copy.segment.start == rawDir.top().segment.start)
                    thisWay.end = rawDir.top().segment.start;
                else if(copy.segment.end == rawDir.top().segment.end || copy.segment.start== rawDir.top().segment.end)
                    thisWay.end = rawDir.top().segment.end;
            }
            
            //Add info to NavSegment
            directions[curEl].m_command = NavSegment::TURN;
            directions[curEl].m_streetName = copy.streetName;
            directions[curEl].m_direction = turnTravel(preWay,thisWay);
            justTurn = true;
            lastName = copy;
            preWay = thisWay;
        }
    }
    
    return NAV_SUCCESS;
}

//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}