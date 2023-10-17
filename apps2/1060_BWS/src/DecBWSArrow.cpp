#include "DecBWSArrow.h"
#include "include/core/SkPath.h"
   
DecBWSArrow::DecBWSArrow(int type) {
    _arrowType = type;
    border( false );
    size( 50, 75 );

    _pStatus = NULL;
}

DecBWSArrow::~DecBWSArrow() {

}

DecBWSArrow &BWSArrow(int type) {
    DecBWSArrow *w = new DecBWSArrow( type );
    return *w;    
}

void DecBWSArrow::_draw_content(SkCanvas& canvas) {
    canvas.save();
    if( _arrowType == 1 ) {
        canvas.scale( 1, -1 );
        canvas.translate( 0, -height() );
    }
    float w = width();
    float h = height();
    float w2 = w / 2;
    float w13 = w * 1 / 3;
    float w23 = w * 2 / 3;

    SkPath path;
    path.moveTo( w2, 1 );
    path.lineTo( w-1, w );
    path.lineTo( w23, w );
    path.lineTo( w23, h-1 );
    path.lineTo( w13, h-1 );
    path.lineTo( w13, w );
    path.lineTo( 1, w );
    path.close();

    SkPaint paint;
    
    paint.setStrokeWidth( 2 );
    paint.setColor( _border_color );

    if( _isActive ) {
        paint.setStyle( SkPaint::Style::kFill_Style );
        canvas.drawPath( path, paint );
    }

    paint.setStyle( SkPaint::Style::kStroke_Style );
    canvas.drawPath( path, paint );
    canvas.restore();
}