/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#include "TDrawHelpers.h"
#include "TMath.h"
#include <string.h>

CTDrawHelpers::CTDrawHelpers(void)
{
}


CTDrawHelpers::~CTDrawHelpers(void)
{
}


TSDWORD CTDrawHelpers::linearScaleImage( CTSurface_RGBA8888 *source, CTSurface_RGBA8888 *target, TSDWORD type )
{
    TSDWORD r,g,b;
    TBYTE xoffset;
    TBYTE yoffset;
    TSDWORD fixedyinc = (source->getHeight()<<8) / target->getHeight();
    TSDWORD fixedxinc = (source->getWidth()<<8) / target->getWidth();
    TSDWORD fixedy=fixedyinc/2;
    TSDWORD p;
    TDWORD *t = (TDWORD*)target->getData();
    TSDWORD y = 0;
    while (y<target->getHeight()) {
        
        TSDWORD fixedx = fixedxinc/2;
        yoffset = (TBYTE)(fixedy&255);
        
        TDWORD *s_horline1 = (TDWORD*)source->getData() + source->getPitch() * (fixedy>>8);
        TDWORD *s_horline2 = s_horline1 + source->getPitch();

        TDWORD *t_horline = t;
        TDWORD *t_horline_target = t+target->getWidth();

        while (t_horline != t_horline_target) {
            p = (fixedx>>8);
            xoffset = (TBYTE)(fixedx&255);

            
            r = ((((( ((s_horline1[p]>>0)&255)*((TBYTE)~xoffset) + ((s_horline1[p+1]>>0)&255)*xoffset ) ) * ((TBYTE)~yoffset) ) + 
                  ((( ((s_horline2[p]>>0)&255)*((TBYTE)~xoffset) + ((s_horline2[p+1]>>0)&255)*xoffset ) ) * (yoffset) )) >> 16);

            g = ((((( ((s_horline1[p]>>8)&255)*((TBYTE)~xoffset) + ((s_horline1[p+1]>>8)&255)*xoffset ) ) * ((TBYTE)~yoffset) ) + 
                  ((( ((s_horline2[p]>>8)&255)*((TBYTE)~xoffset) + ((s_horline2[p+1]>>8)&255)*xoffset ) ) * (yoffset) )) >> 16);

            b = ((((( ((s_horline1[p]>>16)&255)*((TBYTE)~xoffset) + ((s_horline1[p+1]>>16)&255)*xoffset ) ) * ((TBYTE)~yoffset) ) + 
                  ((( ((s_horline2[p]>>16)&255)*((TBYTE)~xoffset) + ((s_horline2[p+1]>>16)&255)*xoffset ) ) * (yoffset) )) >> 16);


            *t_horline = (r | (g<<8) | (b<<16) );


            t_horline++;
            fixedx+=fixedxinc;
        };

        t += target->getPitch();
        y++;;
        fixedy += fixedyinc;
    };

    return 1;
};





void CTDrawHelpers::rotateBlit( CTSurface_RGBA8888*target, CTSurface_RGBA8888 *source,  
                                TSDWORD middlex, TSDWORD middley,TSDWORD vx, TSDWORD vy,
                                TDWORD col, eTBLITOPERATION o )
{
    TSDWORD temp;
    TSDWORD x1,y1,x2,y2,nx,ny;
    TSDWORD leftx,lefty,rightx,righty;



    temp = (source->getWidth()<<FP_BITS) / source->getHeight();
    if (vy>0) {
        nx = tmath_fpmul( -vy, temp );
        ny = tmath_fpmul( vx, temp );
        x1 = (middlex - vx);
        y1 = (middley - vy);
        x2 = (middlex + vx);
        y2 = (middley + vy);
        if (ny>0) {
            leftx = x1+nx;
            lefty = y1+ny;
            rightx = x1-nx+vx*2;
            righty = y1-ny+vy*2;
        } else {
            leftx = x1+nx + vx*2;
            lefty = y1+ny + vy*2;
            rightx = x1-nx;
            righty = y1-ny;
            nx=-nx; ny=-ny;
        }
    } else {
        nx = tmath_fpmul( vy, temp );
        ny = tmath_fpmul( -vx, temp );
        x1 = (middlex + vx);
        y1 = (middley + vy);
        x2 = (middlex - vx);
        y2 = (middley - vy);
        if (ny>0) {
            leftx = x1+nx;
            lefty = y1+ny;
            rightx = x1-nx-vx*2;
            righty = y1-ny-vy*2;
        } else {
            leftx = x1+nx - vx*2;
            lefty = y1+ny - vy*2;
            rightx = x1-nx;
            righty = y1-ny;
            nx=-nx; ny=-ny;
        }
    }
    x1 -= nx; y1 -= ny;
    x2 += nx; y2 += ny;





    TSDWORD leftxinc = tmath_fpdiv( leftx-x1, (lefty-y1)+1 );
    TSDWORD rightxinc = tmath_fpdiv( rightx-x1, (righty-y1)+1 );


    // subpixel, and yclipping
    //TSDWORD intery = (tmath_fixedCeil( y1 )<<FP_BITS) - FP_VAL/2;
    TSDWORD intery = ((y1>>FP_BITS)<<FP_BITS) + FP_VAL/2;
    if (intery<0) intery=0;
    temp = intery-y1;
    TSDWORD interx_left = x1+tmath_fpmul( leftxinc, temp );
    TSDWORD interx_right = x1+tmath_fpmul( rightxinc, temp );





    __int64 juu = ((__int64)vx*(__int64)vx + (__int64)vy*(__int64)vy)>>FP_BITS;
    juu+=(juu>>8);
    vx = (TSDWORD)(((__int64)(vx * source->getHeight())<<13) / (__int64)juu );
    vy = (TSDWORD)(((__int64)(vy * source->getHeight())<<13) / (__int64)juu );




    nx = vy;
    ny = -vx;


    TSDWORD rasx;
    TDWORD *s = (TDWORD*)source->getData();
    TDWORD *t, *t_target;
    // rasterize.
    while (intery<y2 && intery<(target->getHeight()<<FP_BITS)) {
        if (intery>lefty) {
            leftxinc = tmath_fpdiv( x2-leftx,y2-lefty+1 );
            interx_left = leftx + tmath_fpmul( leftxinc, intery-lefty+1 );
            lefty=y2;			// never do this again
        };

        if (intery>righty) {
            rightxinc = tmath_fpdiv( x2-rightx, y2-righty+1 );
            interx_right = rightx + tmath_fpmul( rightxinc, intery-righty+1 );
            righty = y2;		// never do this again.
        };



        x1 = (source->getHeight()<<13) +
             tmath_fpmul( intery-middley, nx ) +
             tmath_fpmul( interx_left-middlex, vx );
        y1 = (source->getWidth()<<13) +
             tmath_fpmul( intery-middley, ny ) +
             tmath_fpmul( interx_left-middlex, vy );


        // NOTE, ADD SUBTEXEL ACCURACY IN INNER LOOP
        rasx = tmath_fixedCeil( interx_left );
        if (rasx<0) rasx = 0;
        temp = (rasx<<FP_BITS)-interx_left;
        x1 += tmath_fpmul( vx, temp );
        y1 += tmath_fpmul( vy, temp );


        temp = tmath_fixedCeil(interx_right);
        if (temp>target->getWidth()) temp = target->getWidth();
        if (temp>rasx) {
            TDWORD c;
            t = (TDWORD*)target->getData() + (intery>>FP_BITS)*target->getPitch();
            t_target = t+temp;
            t+=rasx;

            switch (o) {
            default:



            case eTBLITOPERATION_ALPHA:
            case eTBLITOPERATION_FASTALPHA:
                while (t!=t_target) {
                    {
                        //if (x1>0 && y1>0 && x1<source->getHeight() << FP_BITS && y1<source->getWidth() << FP_BITS) {
                        c= s[ (x1>>FP_BITS) * source->getPitch() + ((y1>>FP_BITS)) ];
                        temp = ((c>>24) * ((col>>24)+1)) >> 8;
                        if (temp>0) {
                            if (temp<255) {
                                *t = (((((c&255)*temp) + ((*t)&255)*(255^temp)) >> 8)) |
                                     ((((((c>>8)&255)*temp) + (((*t)>>8)&255)*(255^temp)) >> 8)<<8) |
                                     ((((((c>>16)&255)*temp) + (((*t)>>16)&255)*(255^temp)) >> 8)<<16);
                            } else {
                                *t = (c&0x00FFFFFF);
                            }
                        };
                        //} else *t = 0xFFFFFFFF;
                    }
                    //*t=0xFFFFFFFF;
                    x1 += vx;
                    y1 += vy;
                    t++;
                };
                break;


				case eTBLITOPERATION_ADD:
                {
                    TSDWORD rr,gg,bb;
                    while (t!=t_target) {
                        //if (x1<0 || y1<0) _asm int 3;
                        //if (x1>=source->getWidth()*FP_VAL || y1>source->getHeight()*FP_VAL) _asm int 3;

                        {
                            c= s[ (x1>>FP_BITS) * source->getPitch() + ((y1>>FP_BITS)) ];
                            rr = ((((c>>0)&255) * ((col>>0)&255) )>>8) + ((*t>>0)&255);
                            if (rr>255) rr= 255;
                            gg = ((((c>>8)&255) * ((col>>8)&255) )>>8) + ((*t>>8)&255);
                            if (gg>255) gg= 255;
                            bb = ((((c>>16)&255) * ((col>>16)&255) )>>8) + ((*t>>16)&255);
                            if (bb>255) bb= 255;

                            *t = rr | (gg<<8) | (bb<<16);

                        }
                        x1 += vx;
                        y1 += vy;
                        t++;
                    };
                }


                break;
            }
        }
        interx_left += leftxinc;
        interx_right += rightxinc;
        intery+=FP_VAL;
    }
};





CTBaseSurface* CTDrawHelpers::qualityShrink( CTBaseSurface *source, TSDWORD sizeDiv, eTSURFACEFORMAT returnFormat ) {
    if (!source) return 0;

    TSDWORD newWidth = source->getWidth() / sizeDiv;
    TSDWORD newHeight = source->getHeight() / sizeDiv;
    if (newWidth<1 || newHeight<1) return 0;
    //if (source->getFormat() != eTSURFACEFORMAT_RGBA8888) return 0;

    CTBaseSurface *rval = new CTBaseSurface();
    rval->create( newWidth, newHeight, returnFormat );

    TSDWORD compmul = 65536 / (sizeDiv*sizeDiv);
    TSDWORD r,g,b,a;
    TDWORD *target = (TDWORD*)rval->getData();
    for (int ty = 0; ty<rval->getHeight(); ty++) {
        switch ( source->getFormat() ) {
        default: break;

        case eTSURFACEFORMAT_RGBA8888:
            {
                TDWORD *s;
                TDWORD *s_target, *sy_target;
                for (int tx = 0; tx<rval->getWidth(); tx++) {
                    r=0;g=0;b=0; a=0;
                    s = ((TDWORD*)source->getData()) + source->getPitch()*ty*sizeDiv + tx*sizeDiv;
                    sy_target = s+source->getPitch()*sizeDiv;
                    while (sy_target!=s) {
                        s_target = s + sizeDiv;
                        while (s!=s_target) {
                            r += (*s&255);
                            g += ((*s>>8)&255);
                            b += ((*s>>16)&255);
                            a += ((*s>>24)&255);
                            s++;
                        };
                        s-=sizeDiv;
                        s+=source->getPitch();
                    }

                    r = ((r*compmul)>>16);
                    g = ((g*compmul)>>16);
                    b = ((b*compmul)>>16);
                    a = ((a*compmul)>>16);

                    //*target = (b>>3) | ((g>>2)<<5) | ((r>>3)<<11);
                    *target = r | (g<<8) | (b<<16) | (a<<24);
                    target++;
                }
            }
            break;

        case eTSURFACEFORMAT_RGB888:
            {
                TBYTE *s;
                TBYTE *s_target, *sy_target;
                for (int tx = 0; tx<rval->getWidth(); tx++) {
                    r=0;g=0;b=0;
                    s = ((TBYTE*)source->getData()) + (source->getPitch()*ty*sizeDiv + tx*sizeDiv)*3;
                    sy_target = s+source->getPitch()*sizeDiv*3;
                    while (sy_target!=s) {
                        s_target = s + sizeDiv*3;
                        while (s!=s_target) {
                            r += (s[0]&255);
                            g += (s[1]&255);
                            b += (s[2]&255);
                            s+=3;
                        };
                        s-=sizeDiv*3;
                        s+=source->getPitch()*3;
                    }

                    r = ((r*compmul)>>16);
                    g = ((g*compmul)>>16);
                    b = ((b*compmul)>>16);

                    *target = (b>>3) | ((g>>2)<<5) | ((r>>3)<<11);
                    target++;
                }
            }
            break;
        }	// endof switch

    };

    return rval;
};
