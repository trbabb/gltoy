/* Image.h
 * 
 * January 25, 2011
 * tbabb
 */

#ifndef GEOMC_IMAGE_H__
#define GEOMC_IMAGE_H__

#include <png.h>
#include <string>
#include <cstdio>
#include <cmath>
#include <boost/shared_array.hpp>
#include <geomc/function/Expr.h>
#include <geomc/linalg/Vec.h>
#include <geomc/random/RandomTools.h>
#include "GraphicsException.h"

//done: this should be a Raster Vec<i,N> -> Vec<T,M> class
//      where the output type is templated (i.e. a bare T for M=1)
//      - have:
//        - RasterField<T,N,M,EdgeBehavior>?
//          > or maybe template on eval()?
//        - fill from png (jpg,exr)
//      - extricate from Expr<>
//      - use a dynamic matrix for storage?
//      - copying/iterator methods, etc.

//redacted: this should probably be a more generic Vec<int,N> -> T "Raster" type.
//  - then an image would be Vec<int,3> -> T  (i.e. x,y,channel -> data)
//  - add methods for slicing-- getting other Raster<>s/Vec<>s out.
//    - e.g. raster.f<M>() -> Vec<int,N-1> -> Vec<T,M>
//    - can dynamically check M against stored last dimension and decide what kind of view to present 

using namespace geom;

#define PNGSIGSIZE 8

bool validate_png(std::istream &source);

/**
 * Image class: T = type of pixel data; N = number of channels
 */
template <typename T, index_t N> 
class Image : public Expr<Vec2i, Vec<T,N> >{
    int _w;
    int _h;
    boost::shared_array<T> data;
    
public:
    Image(int w, int h):_w(w),_h(h){
        data = boost::shared_array<T>(new T[N*_w*_h]);
        clear();
    }
    
    virtual ~Image(){
        //data[] gets free'd automatically.
    }
    
    //todo: eval/set functions which do not check, for private use.

    Vec<T,N> eval(Vec2i pt){
        if (pt.x < 0 || pt.y < 0 || pt.x >= _w || pt.y >= _h){
            return Vec<T,N>(); //zero
        }
        return Vec<T,N>( &(data[index(pt)]) );
    }
    
    Vec<T,N> eval(int x, int y){
        if (x < 0 || y < 0 || x >= _w || y >= _h){
            return Vec<T,N>(); // zero
        }
        return Vec<T,N>( &(data[index(x,y)]));
    }
    
    void set(Vec2i pt, Vec<T,N> px){
        if (pt.x < 0 || pt.y < 0 || pt.x >= _w || pt.y >= _h){
            return;
        }
        int idx = index(pt);
        for (int i = 0; i < N; i++){
            data[idx + i] = px[i];
        }
    }
    
    void set(int x, int y, Vec<T,N> px){
        if (x < 0 || y < 0 || x >= _w || y >= _h){
            return;
        }
        int idx = index(x,y);
        for (int i = 0; i < N; i++){
            data[idx + i] = px[i];
        }
    }
    
    void clear(){
        for (int i = 0; i < _w*_h*N; i++){
            data[i] = 0;
        }
    }
    
    int w(){
        return this->_w;
    }
    
    int h(){
        return this->_h;
    }
    
    const T* getRasterBuffer(){
        return this->data.get();
    }
    
    //TODO: implement filtering here. (not too hard.)
    template <typename I> void fill(boost::shared_ptr< Expr< Vec<I,2>, Vec<T,N> > > expression,
                Rect2d view, int pixelsamples=32){
        Vec2d dims = view.getDimensions();
        Vec2d dPixel = dims / Vec2d(_w,_h);
        Sampler<double> rv;
        for (int y = 0; y < _h; y++){
            for (int x = 0; x < _w; x++){
                Vec<T,N> accum;
                for (int s = 0; s < pixelsamples; s++){
                    Vec2d c1 = view.min() + dims*(Vec2d(x,y)/Vec2d(_w,_h));
                    Vec2d c2 = c1 + dPixel;
                    Vec2d pt = rv.box(c1, c2);
                    accum += expression->eval(Vec<I,2>(pt.x, pt.y));
                }
                this->set(x, y, accum / pixelsamples);
            }
        }
    }
    
    template <typename I> void fill(boost::shared_ptr< Expr< Vec<I,2>, T> > expression,
                Rect2d view, int pixelsamples=32){
        Vec2d dims = view.getDimensions();
        Vec2d dPixel = dims / Vec2d(_w,_h);
        Sampler<double> rv;
        for (int y = 0; y < _h; y++){
            for (int x = 0; x < _w; x++){
                T accum = 0;
                for (int s = 0; s < pixelsamples; s++){
                    Vec2d c1 = view.min() + dims*(Vec2d(x,y)/Vec2d(_w,_h));
                    Vec2d c2 = c1 + dPixel;
                    Vec2d pt = rv.box(c1, c2);
                    accum += expression->eval(Vec<I,2>(pt.x, pt.y));
                }
                this->set(x, y, Vec<T,N>(accum / pixelsamples));
            }
        }
    }
    
    boost::shared_ptr< Expr< Vec2i, Vec<T,N> > > managedCopy(){
        return boost::shared_ptr< Expr< Vec2i, Vec<T,N> > >(new Image<T,N>(*this));
    }
    
    virtual string opname() { return "image"; }
    
    void sout(std::ostream& s){
        s << "Image (" << _w << " x " << _h << ", " << N << " channels)";
    }
    
    //todo: throw some exceptions
    //todo: auto-normalization (watch out for black images)
    bool save_png(std::string path, Vec<T,N> white=(Vec<T,N>::ones), bool sixteenbit=false){
        int imgtype;
        switch (N){
            case 1:
                imgtype = PNG_COLOR_TYPE_GRAY;
                break;
            case 2:
                imgtype = PNG_COLOR_TYPE_GRAY_ALPHA;
                break;
            case 3:
                imgtype = PNG_COLOR_TYPE_RGB;
                break;
            case 4:
                imgtype = PNG_COLOR_TYPE_RGB_ALPHA;
                break;
            default:
                return false;
        }
        png_bytep *img = get_pngdata(white, sixteenbit);
        FILE *fp = fopen(path.c_str(), "wb");
        png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_ptr) throw GraphicsException("could not make png write structure");
        png_infop  info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) throw GraphicsException("could not make png info structure");
        if (setjmp(png_jmpbuf(png_ptr))) throw GraphicsException("png write init failure");
        png_init_io(png_ptr, fp);
        if (setjmp(png_jmpbuf(png_ptr))) throw GraphicsException("png write header failure");
        png_set_IHDR(png_ptr, info_ptr, _w, _h, sixteenbit?16:8, imgtype, PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
        png_write_info(png_ptr, info_ptr);
        if (setjmp(png_jmpbuf(png_ptr))) throw GraphicsException("png write image failure");
        png_write_image(png_ptr, img);
        if (setjmp(png_jmpbuf(png_ptr))) throw GraphicsException("png write completion failure");
        png_write_end(png_ptr, NULL);
        fclose(fp);
        return true;
    }
    
protected:
    
    int index(Vec2i pt){
        return index(pt.x, pt.y);
    }
    
    int index(int x, int y){        
        return (y*_w + x)*N;
    }
    
    png_bytep* get_pngdata(Vec<T,N> white, bool sixteenbit){
        png_bytep *rows = new png_bytep[_h];
        for (int y = 0; y < _h; y++){
            rows[y] = new png_byte[_w*N*(1+sixteenbit)];
            for (int x = 0; x < _w; x++){
                int src_idx = index(x,y);
                //foreach channel:
                for (int c = 0; c < N; c++){
                    T val = data[src_idx+c];
                    val = val < 0 ? 0 : (val > white[c] ? white[c] : val);
                    if (sixteenbit){
                        int ival = (val*0xffff)/white[c];
                        png_byte hi = (ival & 0xff00) >> 8;
                        png_byte lo =  ival & 0x00ff;
                        //MSB first:
                        rows[y][(x*N + c) * 2]     = hi;
                        rows[y][(x*N + c) * 2 + 1] = lo;
                    } else {
                        png_byte b = (val*0xff)/white[c];
                        rows[y][x*N + c] = b;
                    }
                }
            }
        }
        return rows;
    }
};

#endif
