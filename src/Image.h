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
class Image : public Expr<Vec2i, typename PointType<T,N>::point_t > {
    
    /// Type of one pixel in this image.
    typedef typename PointType<T,N>::point_t pixel_t;
    
public:
    
    int _w;
    int _h;
    boost::shared_array<pixel_t> data;
    
public:
    
    Image(int w, int h):_w(w),_h(h){
        data = boost::shared_array<pixel_t>(new pixel_t[_w * _h]);
        clear();
    }
    
    virtual ~Image(){
        // data[] gets free'd automatically.
    }
    
    // todo: eval/set functions which do not check, for private use.
    
    // get raw data (in pixel-contiguous order)
    pixel_t* begin() {
        return data.get();
    }
    
    pixel_t* end() {
        return data.get() + _w * _h;
    }
    
    const pixel_t* begin() const {
        return data.get();
    }
    
    const pixel_t* end() const {
        return data.get() + _w * _h;
    }

    // todo: should be const, but Expr does not declare it that way :|
    pixel_t eval(Vec2i pt) {
        return eval(pt.x, pt.y);
    }
    
    inline pixel_t eval(int x, int y) const {
        if (x < 0 || y < 0 || x >= _w || y >= _h){
            return pixel_t((T)0); // zero
        }
        return data[index(x, y)];
    }
    
    void set(Vec2i pt, pixel_t px) {
        set(pt.x, pt.y, px);
    }
    
    inline void set(int x, int y, pixel_t px) {
        if (x < 0 || y < 0 || x >= _w || y >= _h){
            return;
        }
        data[index(x, y)] = px;
    }
    
    void clear() {
        std::fill(begin(), end(), pixel_t((T)0));
    }
    
    int w() const {
        return this->_w;
    }
    
    int h() const {
        return this->_h;
    }
    
    //TODO: implement filtering here. (not too hard.)
    template <typename I, typename U> 
    void fill(
            boost::shared_ptr< Expr< Vec<I,2>, U > > expression,
            Rect2d view, 
            int pixelsamples=32) {
        Vec2d dims = view.getDimensions();
        Vec2d dPixel = dims / Vec2d(_w,_h);
        Sampler<double> rv;
        for (int y = 0; y < _h; y++){
            for (int x = 0; x < _w; x++){
                pixel_t accum;
                for (int s = 0; s < pixelsamples; s++){
                    Vec2d c1 = view.min() + dims * (Vec2d(x,y) / Vec2d(_w, _h));
                    Vec2d c2 = c1 + dPixel;
                    Vec2d pt = rv.box(c1, c2);
                    accum += pixel_t(expression->eval(Vec<I,2>(pt.x, pt.y)));
                }
                this->set(x, y, accum / pixelsamples);
            }
        }
    }
    
    // ugh, this crap is garbage. when can we get rid of it?
    boost::shared_ptr< Expr< Vec2i, pixel_t > > managedCopy(){
        return boost::shared_ptr< Expr< Vec2i, pixel_t > >(new Image<T,N>(*this));
    }
    
    virtual string opname() { return "image"; }
    
    void sout(std::ostream& s) {
        s << "Image (" << _w << " x " << _h << ", " << N << " channels)";
    }
    
    //todo: throw some exceptions
    //todo: auto-normalization (watch out for black images)
    bool save_png(std::string path, pixel_t white=pixel_t((T)1), bool sixteenbit=false) const {
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
    
    int index(Vec2i pt) const {
        return index(pt.x, pt.y);
    }
    
    int index(int x, int y) const {        
        return y * _w + x;
    }
    
    png_bytep* get_pngdata(pixel_t white, bool sixteenbit) const {
        png_bytep* rows = new png_bytep[_h];
        T* white_p = PointType<T,N>::iterator(white);
        for (int y = 0; y < _h; y++) {
            rows[y] = new png_byte[_w * N * (1+sixteenbit)];
            for (int x = 0; x < _w; x++){
                int src_idx = index(x,y);
                // foreach channel:
                for (int c = 0; c < N; c++) {
                    T val = PointType<T,N>::iterator(data[src_idx])[c];
                    val = val < 0 ? 0 : (val > white_p[c] ? white_p[c] : val);
                    if (sixteenbit) {
                        int ival = (val * 0xffff) / white[c];
                        png_byte hi = (ival & 0xff00) >> 8;
                        png_byte lo =  ival & 0x00ff;
                        // MSB first:
                        rows[y][(x * N + c) * 2]     = hi;
                        rows[y][(x * N + c) * 2 + 1] = lo;
                    } else {
                        png_byte b = (val * 0xff) / white_p[c];
                        rows[y][x * N + c] = b;
                    }
                }
            }
        }
        return rows;
    }
};

#endif
