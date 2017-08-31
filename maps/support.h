//
//  support.h
//  maps
//
//  Created by Tiffany Kyu on 3/14/17.
//  Copyright © 2017 Tiffany Kyu. All rights reserved.
//

#ifndef support_h
#define support_h

inline bool operator <(const GeoCoord &c1, const GeoCoord &c2)  //< overload of GC
{
    if(c1.latitude < c2.latitude)
        return true;
    return false;
}


inline bool operator ==(const GeoCoord &c1, const GeoCoord &c2)    //== overload of GC
{
    if(c1.latitude == c2.latitude && c1.longitude == c2.longitude)
        return true;
    return false;
}


#endif /* support_h */
