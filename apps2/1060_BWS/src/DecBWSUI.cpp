#include "DeclarativeUI.h"
#include "DecBWSUI.h"
#include "DecBWSInfoBox.h"
#include "DecBWSArrow.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

DecBWSUI::DecBWSUI() {
    nWorkingMode = 0;
    updateCount = 0;

    nRedMode = -1;
    bLastDUp = false;
    bLastDDown = false;
    bLeftWeight = true;
    bRightWeight = true;
    fLeftWeight = 10;
    fRightWeight = 10;

    //fLeverSpeed = 1;

    nTrainingMode = 0;
    fMaxSpeed = 1;

    nPosMode = 0;

    fLeftPos = 0;
    fRightPos = 0;

    isTraining = false;

    // Yellow init
    nYelloMode = -1;

    nLeverSpeedMode = -1;
    nFallingLimitMode = -1;
    nPIDMode = -1;    

    fLeftLeverSpeed = 0;
    fRightLeverSpeed = 0;

    fLeftFallingLimit = 0;
    fRightFallingLimit = 0;

    for( int i=0; i<20; i++ ) {
        fPID[i] = 0;
    }

    size( 824, 382 );
    border( false );
    background( false );
    _( Row()
        .size( -1, -1 )
        .border( false )
        // Left bar - panel switch
        ._( Column()
            .size( 80, -1 )
            .border( false )
            // Red Button
            ._( Button( "", 0, [=](DecButton &btn) {
                nWorkingMode = 0;
                _pWorkingPages->activate( 0 );
            })
                .size( -1, -1 )
                .round( 0 )
                .border( false )
                .bg_color( 0xFFf74648 )
                .cache( false )
            )
            // Green Button
            ._( Button( "", 0, [=](DecButton &btn) {
                nWorkingMode = 1;
                _pWorkingPages->activate( 1 );
            })
                .size( -1, -1 )
                .round( 0 )
                .border( false )
                .bg_color( 0xFF2f9b34 )
                .cache( false )
            )
            // Yellow Button
            ._( Button( "", 0, [=](DecButton &btn) {
                nWorkingMode = 2;
                _pWorkingPages->activate( 2 );
            })
                .size( -1, -1 )
                .round( 0 )
                .border( false )
                .bg_color( 0xFFf5ab2b )
                .cache( false )
            )
        )
        // Working Pages
        ._( Pages()
            .ref( (DecWidget **)&_pWorkingPages )
            .border( false )
            .focusable( false )
            .size( -1, -1 )
            // Red Page
            ._( Container()
                .size( -1, -1 )
                .gap( 10 )
                .border( true )
                .border_color( 0xFFf74648 )
                .borderWidth( 5 )  
                .padding( 15, 15, 15, 15 )                
                ._( Row()
                    .size( -1, -1 )
                    .gap( 10 )
                    .border( false )
                    // Lever
                    ._( Column()
                        .ref( (DecWidget **)&_pLeverMode )
                        .size( 0, -1 )
                        .border( true )
                        .border_color( 0xFFf74648 )
                        .borderWidth( 2 )
                        .padding( 5, 10, 10, 10 )
                        .background( false )
                        .gap( 15 )
                        .onMousePressed( [=]( DecWidget &widget ) -> bool {
                            SkDebugf( "Mode Lever\n" );
                            //setBWSMode( 101 );
                            nRedMode = 0;
                            _pLeverMode->borderWidth( 4 );
                            _pWeightSupportMode->borderWidth( 2 );
                            _pTrainingMode->borderWidth( 2 );
                            return false;
                        })
                        ._( Stack().size( 120, 0 ).border( false ) 
                            ._( Text( "Lever" ).align( ALIGN_CENTER ))
                        )
                        ._( Spacer().size( 0, -1 ) )
                        ._( BWSArrow( 0 )
                            .ref( &pArrowLeverUp )
                            .align( ALIGN_CENTER ) 
                            .border_color( 0xFFf74648 )
                        )
                        ._( BWSInfoBox() 
                            .ref( (DecWidget **)&pInfoLeverSpeed )
                            .text( "Speed (m/s)" )
                            .size( 120, 44 )
                            .border_color( 0xFFf74648 )
                        )
                        ._( BWSArrow( 1 ) 
                            .ref( &pArrowLeverDown )
                            .align( ALIGN_CENTER ) 
                            .border_color( 0xFFf74648 )
                        )
                        ._( Spacer().size( 0, -1 ) )
                    )
                    // Weight support
                    ._( Column()
                        .ref( (DecWidget **)&_pWeightSupportMode )
                        .size( -1, -1 )
                        .border( true )
                        .border_color( 0xFFf74648 )
                        .borderWidth( 2 )
                        .padding( 5, 10, 10, 10 )
                        .background( false )
                        .gap( 15 )
                        .onMousePressed( [=]( DecWidget &widget ) -> bool {
                            SkDebugf( "Mode Weight\n" );
                            //setBWSMode( 102 );
                            nRedMode = 1;
                            _pLeverMode->borderWidth( 2 );
                            _pWeightSupportMode->borderWidth( 4 );
                            _pTrainingMode->borderWidth( 2 );
                            return false;
                        })                        
                        ._( Stack().size( -1, 0 ).border( false ) 
                            ._( Text( "Weight support (kg)" ).align( ALIGN_CENTER ))
                        )                        
                        ._( Spacer().size( 0, -1 ) )
                        ._( Row()
                            .size( -1, 0 )
                            .gap( 10 )
                            .border( false )
                            ._( Column()
                                .size( -1, 0 )
                                .border( false )
                                .gap( 15 )
                                .align( ALIGN_CENTER )
                                ._( Stack() 
                                    .size( -1, 0 )
                                    .border( false )
                                    ._( BWSInfoBox()
                                        .ref( (DecWidget **)&pLeftWeightSupportButton )
                                        .active( true )
                                        .size( 44, 44 )
                                        .border(true )
                                        .borderWidth( 2 )
                                        .round( 22 )
                                        .border_color( 0xFFf74648 )
                                        .align( ALIGN_CENTER )
                                        .bg_color( 0xFFf74648 )
                                        .onMousePressed( [=]( DecWidget &widget ) -> bool {
                                            bLeftWeight = !bLeftWeight;
                                            if( bLeftWeight ) {
                                                widget.bg_color( 0xFFf74648 );
                                            }
                                            else {
                                                widget.bg_color( 0xFFFFFFFF );
                                            }
                                            return false;
                                        })
                                    )
                                )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pLeftWeightSupportText )
                                    .text( "Left (kg)")
                                    .size( -1, 44 )
                                    .border_color( 0xFFf74648 )
                                )
                                ._( Spacer().size( 0, 44 ) )
                            )
                            ._( Column()
                                .size( 0, 0 )
                                .border( false )
                                .gap( 15 )
                                ._( BWSArrow(0) 
                                    .ref( &pArrowWeightSupportUp )
                                    .border_color( 0xFFf74648 )
                                )
                                ._( BWSArrow(1) 
                                    .ref( &pArrowWeightSupportDown )
                                    .border_color( 0xFFf74648 )
                                )
                            )
                            ._( Column()
                                .size( -1, 0 )
                                .border( false )
                                .gap( 15 )
                                .align( ALIGN_CENTER )
                                ._( Stack() 
                                    .size( -1, 0 )
                                    .border( false )
                                    ._( BWSInfoBox()
                                        .ref( (DecWidget **)&pRightWeightSupportButton )
                                        .active( true )
                                        .size( 44, 44 )
                                        .border(true )
                                        .borderWidth( 2 )
                                        .round( 22 )
                                        .border_color( 0xFFf74648 )
                                        .align( ALIGN_CENTER )
                                        .bg_color( 0xFFf74648 )
                                        .onMousePressed( [=]( DecWidget &widget ) -> bool {
                                            bRightWeight = !bRightWeight;
                                            if( bRightWeight ) {
                                                widget.bg_color( 0xFFf74648 );
                                            }
                                            else {
                                                widget.bg_color( 0xFFFFFFFF );
                                            }
                                            return false;
                                        })

                                    )
                                )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pRightWeightSupportText )
                                    .text( "Right (kg)")
                                    .size( -1, 44 )
                                    .border_color( 0xFFf74648 )
                                )
                                ._( Spacer().size( 0, 44 ) )
                            )

                        )
                        ._( Stack()
                            .size( -1, 0 )
                            .border( false )
                            // Reset Button
                            ._( Button( "Reset", 0, [=](DecWidget &widget) {
                                if( bLeftWeight )
                                    fLeftWeight = 0;
                                if( bRightWeight )
                                    fRightWeight = 0;
                                char sz[32];
                                sprintf( sz, "L %0.1f kg", fLeftWeight );
                                pLeftWeightSupportText->text( sz );
                                sprintf( sz, "R %0.1f kg", fRightWeight );
                                pRightWeightSupportText->text( sz );                                    
                            } ) 
                                .text( "Reset" )
                                .size( -1, 44 )
                                .border_color( 0xFFf74648 )
                                .borderWidth( 2 )
                                .bg_color( 0xFFFFFFFF )
                                .round( 0 )
                                .align( ALIGN_CENTER )
                            )                                
                        )                        
                        ._( Spacer().size( 0, -1 ) )
                    )
                    // Training mode
                    ._( Column()
                        .ref( (DecWidget **)&_pTrainingMode )
                        .size( 0, -1 )
                        .border( true )
                        .border_color( 0xFFf74648 )
                        .borderWidth( 2 )
                        .padding( 5, 10, 10, 10 )
                        .background( false )
                        .gap( 0 )
                        .onMousePressed( [=]( DecWidget &widget ) -> bool {
                            SkDebugf( "Mode Training\n" );
                            nRedMode = 2;
                            _pLeverMode->borderWidth( 2 );
                            _pWeightSupportMode->borderWidth( 2 );
                            _pTrainingMode->borderWidth( 4 );
                            return false;
                        })                        
                        ._( Stack().size( 230, 0 ).border( false ) 
                            ._( Text( "Training mode" ).align( ALIGN_CENTER ))
                        )                        
                        ._( Spacer().size( 0, -1 ) )
                        ._( Row()
                            .size( 0, 0 )
                            .gap( 10 )
                            .border( false )
                            ._( Stack() 
                                .size( 110, 0 )
                                .border( false )
                                ._( Text("Mode").align( ALIGN_CENTER ))
                            )
                            ._( Stack() 
                                .size( 110, 0 )
                                .border( false )
                                ._( Text("Max speed").align( ALIGN_CENTER ))
                            )
                        )
                        ._( Spacer().size( 0, -1 ) )
                        ._( Row()
                            .size( 0, 0 )
                            .gap( 10 )
                            .border( false )
                            ._( Column()
                                .size( 0, 0 )
                                .border( false )
                                .gap( 15 )
                                .align( ALIGN_CENTER )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pTrainingModeStraight )
                                    .text( "Straight" )
                                    .size( 110, 44 )
                                    .border_color( 0xFFf74648 )
                                    .borderWidth( 3 )
                                    .onMousePressed( [=](DecWidget &w) -> bool {
                                        if( bR2 ) {
                                            setTrainingMode( 0 );
                                        }
                                        return true;
                                    })
                                )                                
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pTrainingModeFollow )
                                    .text( "Following" )
                                    .size( 110, 44 )
                                    .border_color( 0xFFf74648 )
                                    .onMousePressed( [=](DecWidget &w) -> bool {
                                        if( bR2 ) {
                                            setTrainingMode( 1 );
                                        }
                                        return true;
                                    })                                    
                                )                                
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pTrainingModeManual )
                                    .text( "Manual" )
                                    .size( 110, 44 )
                                    .border_color( 0xFFf74648 )
                                    .onMousePressed( [=](DecWidget &w) -> bool {
                                        if( bR2 ) {
                                            setTrainingMode( 2 );
                                        }
                                        return true;
                                    })                                )                                
                            )
                            ._( Column()
                                .size( 0, 0 )
                                .border( false )
                                .gap( 15 )
                                .align( ALIGN_CENTER )
                                ._( BWSArrow() 
                                    .ref( &pArrowMaxSpeedUp )
                                    .align( ALIGN_CENTER )
                                    .border_color( 0xFFf74648 )
                                )        
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pTrainingMaxSpeedBox )
                                    .text( "Speed (m/s)" )
                                    .size( 110, 44 )
                                    .border_color( 0xFFf74648 )
                                )                                                         
                                ._( BWSArrow(1) 
                                    .ref( &pArrowMaxSpeedDown )
                                    .align( ALIGN_CENTER )
                                    .border_color( 0xFFf74648 )
                                )                                
                            )
                        )
                        ._( Spacer().size( 0, -1 ) )
                    )
                )
            )
            // Green Page
            ._( Container()
                .size( -1, -1 )
                .gap( 10 )
                .border( true )
                .border_color( 0xFF2f9b34 )
                .borderWidth( 5 )  
                .padding( 15, 15, 15, 15 )        
                ._( Row()
                    .size( -1, -1 )
                    .gap( 10 )
                    .border( false )
                    // Turning
                    ._(Column()
                        .ref( (DecWidget **)&_pTurningMode )
                        .size( 0, -1 )
                        .border( true )
                        .border_color( 0xFF2f9b34 )
                        .borderWidth( 2 )                     
                        .padding( 5, 10, 10, 10 )   
                        .onMousePressed( [=]( DecWidget &widget ) -> bool {
                            SkDebugf( "Mode Turning\n" );
                            nGreenMode = 0;
                            _pTurningMode->borderWidth( 4 );
                            _pTrainingMode2->borderWidth( 2 );
                            _pStatusMode->borderWidth( 2 );
                            return false;
                        })                          
                        ._( Stack().size( 150, 0 ).border( false ) 
                            ._( Text( "Turning offset" ).align( ALIGN_CENTER ))
                        )                            
                        ._(Row()
                            .size( 0, -1 )
                            .gap( 10 )  
                            .border( false )                
                            ._( Column()
                                .size( 0, 0 )
                                .border( false )
                                .gap( 15 )
                                .align( ALIGN_CENTER )
                                ._( BWSArrow() 
                                    .ref( (DecWidget **)&pArrowLeftPosUp )
                                    .align( ALIGN_CENTER )
                                    .border_color( 0xFF2f9b34 )
                                )        
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoLeftPos )
                                    .text( "L Pos (cm)" )
                                    .size( 70, 44 )
                                    .borderWidth( 3 )
                                    .border_color( 0xFF2f9b34 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPosMode = 0;
                                        pInfoLeftPos->borderWidth( 3 );
                                        pInfoRightPos->borderWidth( 1 );
                                        return true;
                                    })
                                )                                                         
                                ._( BWSArrow(1) 
                                    .ref( (DecWidget **)&pArrowLeftPosDown )
                                    .align( ALIGN_CENTER )
                                    .border_color( 0xFF2f9b34 )
                                )                                
                            )           
                            ._( Column()
                                .size( 0, 0 )
                                .border( false )
                                .gap( 15 )
                                .align( ALIGN_CENTER )
                                ._( BWSArrow() 
                                    .ref( (DecWidget **)&pArrowRightPosUp )
                                    .align( ALIGN_CENTER )
                                    .border_color( 0xFF2f9b34 )
                                )        
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoRightPos )
                                    .text( "R pos (cm)" )
                                    .size( 70, 44 )
                                    .border_color( 0xFF2f9b34 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPosMode = 1;
                                        pInfoLeftPos->borderWidth( 1 );
                                        pInfoRightPos->borderWidth( 3 );
                                        return true;
                                    })
                                )                                                         
                                ._( BWSArrow(1) 
                                    .ref( (DecWidget **)&pArrowRightPosDown )
                                    .align( ALIGN_CENTER )
                                    .border_color( 0xFF2f9b34 )
                                )                                
                            )                       
                        )                    
                    )
                    // Training
                    ._( Column()
                        .ref( (DecWidget **)&_pTrainingMode2 )
                        .size( 310, -1 )
                        .border( true )
                        .border_color( 0xFF2f9b34 )
                        .borderWidth( 2 )
                        .padding( 5, 10, 10, 10 )
                        .background( false )
                        .onMousePressed( [=]( DecWidget &widget ) -> bool {
                            SkDebugf( "Mode Training\n" );
                            nGreenMode = 1;
                            _pTurningMode->borderWidth( 2 );
                            _pTrainingMode2->borderWidth( 4 );
                            _pStatusMode->borderWidth( 2 );
                            return false;
                        })                         
                        ._( Stack().size( 290, 0 ).border( false ) 
                            ._( Text( "Training" ).align( ALIGN_CENTER ))
                        )                        
                        ._( Spacer().size( 0, -1 ) )
                        ._( Row()
                            .size( 0, 0 )
                            .gap( 10 )
                            .border( false )
                            ._( Column()
                                .size( 0, 0 )
                                .border( false )
                                ._( Stack().size( 70, 0 ).border( false ) 
                                    ._( Text( "Mode" )
                                        .align( ALIGN_CENTER )
                                    )
                                )                                 
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&_pTextModeName )
                                    .size( 70, 44 )
                                    .border_color( 0xFF2f9b34 )
                                )
                            )
                            ._( Column()
                                .size( 0, 0 )
                                .border( false )
                                ._( Stack().size( 130, 0 ).border( false ) 
                                    ._( Text( "W.support" ).align( ALIGN_CENTER ))
                                )                                 
                                ._( Row() 
                                    .size( -1, 0 )
                                    ._( BWSInfoBox() 
                                        .ref( (DecWidget**)&pLeftWeightSupportText2 )
                                        .size( 65, 44 )
                                        .border_color( 0xFF2f9b34 )
                                    )
                                    ._( BWSInfoBox() 
                                        .ref( (DecWidget **)&pRightWeightSupportText2 )
                                        .size( 65, 44 )
                                        .border_color( 0xFF2f9b34 )
                                    )
                                )
                            )
                            ._( Column()
                                .size( 0, 0 )
                                .border( false )
                                ._( Stack().size( 70, 0 ).border( false ) 
                                    ._( Text( "Max speed" ).align( ALIGN_CENTER ))
                                )                                 
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pTrainingMaxSpeedBox2 )
                                    .size( 70, 44 )
                                    .border_color( 0xFF2f9b34 )
                                )
                            )                            
                        )
                        ._( Spacer().size( 0, -1 ) )
                        ._( Row()
                            .size( -1, 0 )
                            .border( false )
                            ._( Spacer().size( -1, 0 ) )
                            // Play Button
                            ._( Button( "", 0xf04b , [=] (DecButton &btn) -> bool {
                                if( bR2 ) {
                                    isTraining = true;
                                    _pButtonStartTraining->text_color( 0xFF2f9b34 );
                                    _pButtonStopTraining->text_color( 0xFFC0C0C0 );
                                    setBWSMode( 1, 1 ); 
                                }
                                return true;
                            })
                                .fontSize( 80 )
                                .border( false )
                                .background( false )
                                .text_color( 0xFFC0C0C0 )
                                .width( 80 )
                                .ref( (DecWidget **)&_pButtonStartTraining )
                            )
                            ._( Spacer().size( 20, 0 ) )
                            // Pause Button
                            ._( Button( "", 0xf04c, [=] (DecButton &btn) -> bool {
                                if( bR2 ) {
                                    isTraining = false;
                                    _pButtonStartTraining->text_color( 0xFFC0C0C0 );
                                    _pButtonStopTraining->text_color( 0xFF2f9b34 );
                                    setBWSMode( 1, 0 ); 
                                }
                                return true;
                            })
                                .fontSize( 80 ) 
                                .border( false )
                                .background( false )
                                .width(80)
                                .text_color( 0xFF2f9b34 )
                                .ref( (DecWidget **)&_pButtonStopTraining )
                            )
                            ._( Spacer().size( -1, 0 ) )
                        )
                    )   
                    // Status
                    ._( Column()
                        .ref( (DecWidget **)&_pStatusMode )
                        .size( -1, -1 )
                        .border( true )
                        .border_color( 0xFF2f9b34 )
                        .borderWidth( 2 )
                        .padding( 5, 10, 10, 10 )
                        .background( false )
                        .onMousePressed( [=]( DecWidget &widget ) -> bool {
                            SkDebugf( "Mode Status\n" );
                            nGreenMode = 2;
                            _pTurningMode->borderWidth( 2 );
                            _pTrainingMode2->borderWidth( 2 );
                            _pStatusMode->borderWidth( 4 );
                            return false;
                        })                         
                        ._( Stack().size( -1, 0 ).border( false ) 
                            ._( Text( "Status" ).align( ALIGN_CENTER ))
                        )                          
                        ._( Stack().size( -1, 0 )
                            .border( false ) 
                            .margin_top( 20 )
                            .margin_bottom( 10 )
                            ._( Text( "Limit Switch" ).align( ALIGN_CENTER ))
                        )                          
                        ._( Row()
                            .size( -1, 0 )
                            .gap( 10 )
                            .border( false )
                            ._( Column() 
                                .size( -1, 0 )
                                .border( false )
                                .gap( 15 )
                                ._( Row() 
                                    .size( -1, 0 )
                                    .gap( 10 )
                                    .border( 0 )
                                    ._(BWSInfoBox().size( 32, 32 ).round( 16 ).border_color(0xFF2f9b34) )
                                    ._(Text( "L1" ).align(ALIGN_CENTER ) )
                                )
                                ._( Row() 
                                    .size( -1, 0 )
                                    .gap( 10 )
                                    .border( 0 )
                                    ._(BWSInfoBox().size( 32, 32 ).round( 16 ).border_color(0xFF2f9b34) )
                                    ._(Text( "L2" ).align(ALIGN_CENTER ) )
                                )
                                ._( Row() 
                                    .size( -1, 0 )
                                    .gap( 10 )
                                    .border( 0 )
                                    ._(BWSInfoBox().size( 32, 32 ).round( 16 ).border_color(0xFF2f9b34) )
                                    ._(Text( "L3" ).align(ALIGN_CENTER ) )
                                )
                                ._( Row() 
                                    .size( -1, 0 )
                                    .gap( 10 )
                                    .border( 0 )
                                    ._(BWSInfoBox().size( 32, 32 ).round( 16 ).border_color(0xFF2f9b34) )
                                    ._(Text( "L4" ).align(ALIGN_CENTER ) )
                                )
                            )
                            ._( Column() 
                                .size( -1, 0 )
                                .border( false )
                                .gap( 15 )
                                ._( Row() 
                                    .size( -1, 0 )
                                    .gap( 10 )
                                    .border( 0 )
                                    ._(BWSInfoBox().size( 32, 32 ).round( 16 ).border_color(0xFF2f9b34) )
                                    ._(Text( "R1" ).align(ALIGN_CENTER ) )
                                )
                                ._( Row() 
                                    .size( -1, 0 )
                                    .gap( 10 )
                                    .border( 0 )
                                    ._(BWSInfoBox().size( 32, 32 ).round( 16 ).border_color(0xFF2f9b34) )
                                    ._(Text( "R2" ).align(ALIGN_CENTER ) )
                                )
                                ._( Row() 
                                    .size( -1, 0 )
                                    .gap( 10 )
                                    .border( 0 )
                                    ._(BWSInfoBox().size( 32, 32 ).round( 16 ).border_color(0xFF2f9b34) )
                                    ._(Text( "R3" ).align(ALIGN_CENTER ) )
                                )
                                ._( Row() 
                                    .size( -1, 0 )
                                    .gap( 10 )
                                    .border( 0 )
                                    ._(BWSInfoBox().size( 32, 32 ).round( 16 ).border_color(0xFF2f9b34) )
                                    ._(Text( "R4" ).align(ALIGN_CENTER ) )
                                )
                            )
                        )
                        ._( Stack().size( -1, 0 )
                            .border( false ) 
                            .margin_top( 20 )
                            .margin_bottom( 10 )
                            ._( Text( "Emergency" ).align( ALIGN_CENTER ))
                        )                          
                        ._( Row()
                            .size( -1, 50 )
                            .gap( 10 )
                            .border( false )
                            ._( Column() 
                                .size( -1, 44 )
                                .border( false )
                                .gap( 15 )
                                ._( Row() 
                                    .size( -1, 0 )
                                    .gap( 10 )
                                    .border( 0 )
                                    ._(BWSInfoBox().size( 32, 32 ).round( 16 ).border_color(0xFF2f9b34) )
                                    ._(Text( "L1" ).align(ALIGN_CENTER ) )
                                )
                            )
                            ._( Column() 
                                .size( -1, 44 )
                                .border( false )
                                .gap( 15 )
                                ._( Row() 
                                    .size( -1, 0 )
                                    .gap( 10 )
                                    .border( 0 )
                                    ._(BWSInfoBox().size( 32, 32 ).round( 16 ).border_color(0xFF2f9b34) )
                                    ._(Text( "R1" ).align(ALIGN_CENTER ) )
                                )
                            )
                        )                        
                    )                                                         
                )
            )
            ._( Column()
                .size( -1, -1 )
                .gap( 10 )
                .border( true )
                .border_color( 0xFFf5ab2b )
                .borderWidth( 5 )  
                .padding( 15, 15, 15, 15 )                   
                // Yellow Page
                ._( Row()
                    .size( -1, -1 )
                    .gap( 10 )
                    .border( false )
                    ._( Column()
                        .ref( (DecWidget **)&_pLeverSpeedMode )
                        .size( 0, -1 )
                        .border( true )
                        .border_color( 0xFFf5ab2b )
                        .borderWidth( 2 )
                        .padding( 10, 10, 10, 10 )
                        .background( false )
                        .onMousePressed( [=]( DecWidget &widget ) -> bool {
                            SkDebugf( "Mode Lever Speed (Yellow 0)\n" );
                            nYelloMode = 0;
                            _pLeverSpeedMode->borderWidth( 4 );
                            _pFallingLimitMode->borderWidth( 2 );
                            _pPIDMode->borderWidth( 2 );
                            return false;
                        })                        
                        ._( Stack().size( 170, 0 ).border( false ) 
                            ._( Text( "Lever Speed" ).align( ALIGN_CENTER ))
                        )  
                        ._( Spacer().size( 0, -1 ) )
                        ._( Row()
                            .size( 170, 0 )
                            .gap( 10 )
                            .border( false )
                            ._( Stack().size( -1, 0 ).border( false ) 
                                ._( Text( "Left (m/s)" ).align( ALIGN_CENTER ))
                            ) 
                            ._( Stack().size( -1, 0 ).border( false ) 
                                ._( Text( "Right (m/s)" ).align( ALIGN_CENTER ))
                            ) 
                        )
                        ._( Row()
                            .size( 0, 0 )
                            .gap( 10 )
                            .border( false )
                            ._( BWSInfoBox() 
                                .ref( (DecWidget **)&pInfoLeftLeverSpeed)
                                .size( 80, 44 )
                                .border_color( 0xFFf5ab2b )
                                .onMousePressed( [=]( DecWidget &w ) -> bool {
                                    nLeverSpeedMode = 0;
                                    YellowSetUIValues();
                                    YellowGetKeyPadString();
                                    pInfoLeftLeverSpeed->borderWidth( 3 );
                                    pInfoRightLeverSpeed->borderWidth( 1 );
                                    pInfoLeftFallingLimit->borderWidth( 1 );
                                    pInfoRightFallingLimit->borderWidth( 1 );
                                    pInfoLeftWheelP->borderWidth( 1 );
                                    pInfoLeftWheelI->borderWidth( 1 );
                                    pInfoLeftWheelD->borderWidth( 1 );
                                    pInfoRightWheelP->borderWidth( 1 );
                                    pInfoRightWheelI->borderWidth( 1 );
                                    pInfoRightWheelD->borderWidth( 1 );
                                    pInfoLeftBWSP->borderWidth( 1 );
                                    pInfoLeftBWSI->borderWidth( 1 );
                                    pInfoLeftBWSD->borderWidth( 1 );
                                    pInfoRightBWSP->borderWidth( 1 );
                                    pInfoRightBWSI->borderWidth( 1 );
                                    pInfoRightBWSD->borderWidth( 1 );
                                    pInfoLeftRefA->borderWidth( 1 );
                                    pInfoLeftRefB->borderWidth( 1 );
                                    pInfoLeftRefC->borderWidth( 1 );
                                    pInfoLeftRefD->borderWidth( 1 );
                                    pInfoRightRefA->borderWidth( 1 );
                                    pInfoRightRefB->borderWidth( 1 );
                                    pInfoRightRefC->borderWidth( 1 );
                                    pInfoRightRefD->borderWidth( 1 );
                                    return true;
                                })                                
                            )
                            ._( BWSInfoBox() 
                                .ref( (DecWidget **)&pInfoRightLeverSpeed)
                                .size( 80, 44 )
                                .border_color( 0xFFf5ab2b )
                                .onMousePressed( [=]( DecWidget &w ) -> bool {
                                    nLeverSpeedMode = 1;
                                    YellowSetUIValues();
                                    YellowGetKeyPadString();
                                    pInfoLeftLeverSpeed->borderWidth( 1 );
                                    pInfoRightLeverSpeed->borderWidth( 3 );
                                    pInfoLeftFallingLimit->borderWidth( 1 );
                                    pInfoRightFallingLimit->borderWidth( 1 );
                                    pInfoLeftWheelP->borderWidth( 1 );
                                    pInfoLeftWheelI->borderWidth( 1 );
                                    pInfoLeftWheelD->borderWidth( 1 );
                                    pInfoRightWheelP->borderWidth( 1 );
                                    pInfoRightWheelI->borderWidth( 1 );
                                    pInfoRightWheelD->borderWidth( 1 );
                                    pInfoLeftBWSP->borderWidth( 1 );
                                    pInfoLeftBWSI->borderWidth( 1 );
                                    pInfoLeftBWSD->borderWidth( 1 );
                                    pInfoRightBWSP->borderWidth( 1 );
                                    pInfoRightBWSI->borderWidth( 1 );
                                    pInfoRightBWSD->borderWidth( 1 );
                                    pInfoLeftRefA->borderWidth( 1 );
                                    pInfoLeftRefB->borderWidth( 1 );
                                    pInfoLeftRefC->borderWidth( 1 );
                                    pInfoLeftRefD->borderWidth( 1 );
                                    pInfoRightRefA->borderWidth( 1 );
                                    pInfoRightRefB->borderWidth( 1 );
                                    pInfoRightRefC->borderWidth( 1 );
                                    pInfoRightRefD->borderWidth( 1 );
                                    return true;
                                })                                
                            )
                        )
                        ._( Spacer().size( 0, -1 ) )
                    )   
                    ._( Column()
                        .ref( (DecWidget **)&_pPIDMode )
                        .size( -1, -1 )
                        .border( true )
                        .border_color( 0xFFf5ab2b )
                        .borderWidth( 2 )
                        .padding( 10, 10, 10, 10 )
                        .background( false )
                        .gap( 10 )
                        .onMousePressed( [=]( DecWidget &widget ) -> bool {
                            SkDebugf( "Mode PID (Yellow 2)\n" );
                            nYelloMode = 2;
                            _pLeverSpeedMode->borderWidth( 2 );
                            _pFallingLimitMode->borderWidth( 2 );
                            _pPIDMode->borderWidth( 4 );
                            return false;
                        })                        
                        ._( Stack().size( -1, 0 ).border( false ) 
                            ._( Text( "Control Parameters" ).align( ALIGN_CENTER ))
                        )  
                        ._( Row() 
                            .size( -1, 0 )
                            .border( false )
                            ._( Stack().size( 50, 0 ).border( false ) 
                            )                              
                            ._( Stack().size( -1, 0 ).border( false ) 
                                ._( Text( "Left" ).align( ALIGN_CENTER ))
                            )  
                            ._( Stack().size( -1, 0 ).border( false ) 
                                ._( Text( "Right" ).align( ALIGN_CENTER ))
                            )  
                        )
                        ._( Row() 
                            .size( -1, 0 )
                            .gap( 10 )
                            .border( false )
                            ._( Stack().size( 50, 0 ).border( false ) 
                                ._( Text( "Wheel" ).align( ALIGN_VCENTERLEFT ))
                            )  
                            ._(Row()
                                .gap( 2 )
                                .border( false )
                                .size( -1, 0 )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoLeftWheelP )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 0;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 3 );
                                        pInfoLeftWheelI->borderWidth( 1 );
                                        pInfoLeftWheelD->borderWidth( 1 );
                                        pInfoRightWheelP->borderWidth( 1 );
                                        pInfoRightWheelI->borderWidth( 1 );
                                        pInfoRightWheelD->borderWidth( 1 );
                                        pInfoLeftBWSP->borderWidth( 1 );
                                        pInfoLeftBWSI->borderWidth( 1 );
                                        pInfoLeftBWSD->borderWidth( 1 );
                                        pInfoRightBWSP->borderWidth( 1 );
                                        pInfoRightBWSI->borderWidth( 1 );
                                        pInfoRightBWSD->borderWidth( 1 );
                                        pInfoLeftRefA->borderWidth( 1 );
                                        pInfoLeftRefB->borderWidth( 1 );
                                        pInfoLeftRefC->borderWidth( 1 );
                                        pInfoLeftRefD->borderWidth( 1 );
                                        pInfoRightRefA->borderWidth( 1 );
                                        pInfoRightRefB->borderWidth( 1 );
                                        pInfoRightRefC->borderWidth( 1 );
                                        pInfoRightRefD->borderWidth( 1 );
                                        return true;
                                    })                                
                                )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoLeftWheelI )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 1;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 1 );
                                        pInfoLeftWheelI->borderWidth( 3 );
                                        pInfoLeftWheelD->borderWidth( 1 );
                                        pInfoRightWheelP->borderWidth( 1 );
                                        pInfoRightWheelI->borderWidth( 1 );
                                        pInfoRightWheelD->borderWidth( 1 );
                                        pInfoLeftBWSP->borderWidth( 1 );
                                        pInfoLeftBWSI->borderWidth( 1 );
                                        pInfoLeftBWSD->borderWidth( 1 );
                                        pInfoRightBWSP->borderWidth( 1 );
                                        pInfoRightBWSI->borderWidth( 1 );
                                        pInfoRightBWSD->borderWidth( 1 );
                                        pInfoLeftRefA->borderWidth( 1 );
                                        pInfoLeftRefB->borderWidth( 1 );
                                        pInfoLeftRefC->borderWidth( 1 );
                                        pInfoLeftRefD->borderWidth( 1 );
                                        pInfoRightRefA->borderWidth( 1 );
                                        pInfoRightRefB->borderWidth( 1 );
                                        pInfoRightRefC->borderWidth( 1 );
                                        pInfoRightRefD->borderWidth( 1 );
                                        return true;
                                    })                                
                                )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoLeftWheelD )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 2;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 1 );
                                        pInfoLeftWheelI->borderWidth( 1 );
                                        pInfoLeftWheelD->borderWidth( 3 );
                                        pInfoRightWheelP->borderWidth( 1 );
                                        pInfoRightWheelI->borderWidth( 1 );
                                        pInfoRightWheelD->borderWidth( 1 );
                                        pInfoLeftBWSP->borderWidth( 1 );
                                        pInfoLeftBWSI->borderWidth( 1 );
                                        pInfoLeftBWSD->borderWidth( 1 );
                                        pInfoRightBWSP->borderWidth( 1 );
                                        pInfoRightBWSI->borderWidth( 1 );
                                        pInfoRightBWSD->borderWidth( 1 );
                                        pInfoLeftRefA->borderWidth( 1 );
                                        pInfoLeftRefB->borderWidth( 1 );
                                        pInfoLeftRefC->borderWidth( 1 );
                                        pInfoLeftRefD->borderWidth( 1 );
                                        pInfoRightRefA->borderWidth( 1 );
                                        pInfoRightRefB->borderWidth( 1 );
                                        pInfoRightRefC->borderWidth( 1 );
                                        pInfoRightRefD->borderWidth( 1 );
                                        return true;
                                    })                                
                                )
                            )
                            ._(Row()
                                .gap( 2 )
                                .border( false )
                                .size( -1, 0 )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoRightWheelP )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 3;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 1 );
                                        pInfoLeftWheelI->borderWidth( 1 );
                                        pInfoLeftWheelD->borderWidth( 1 );
                                        pInfoRightWheelP->borderWidth( 3 );
                                        pInfoRightWheelI->borderWidth( 1 );
                                        pInfoRightWheelD->borderWidth( 1 );
                                        pInfoLeftBWSP->borderWidth( 1 );
                                        pInfoLeftBWSI->borderWidth( 1 );
                                        pInfoLeftBWSD->borderWidth( 1 );
                                        pInfoRightBWSP->borderWidth( 1 );
                                        pInfoRightBWSI->borderWidth( 1 );
                                        pInfoRightBWSD->borderWidth( 1 );
                                        pInfoLeftRefA->borderWidth( 1 );
                                        pInfoLeftRefB->borderWidth( 1 );
                                        pInfoLeftRefC->borderWidth( 1 );
                                        pInfoLeftRefD->borderWidth( 1 );
                                        pInfoRightRefA->borderWidth( 1 );
                                        pInfoRightRefB->borderWidth( 1 );
                                        pInfoRightRefC->borderWidth( 1 );
                                        pInfoRightRefD->borderWidth( 1 );
                                        return true;
                                    })                                
                                )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoRightWheelI )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 4;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 1 );
                                        pInfoLeftWheelI->borderWidth( 1 );
                                        pInfoLeftWheelD->borderWidth( 1 );
                                        pInfoRightWheelP->borderWidth( 1 );
                                        pInfoRightWheelI->borderWidth( 3 );
                                        pInfoRightWheelD->borderWidth( 1 );
                                        pInfoLeftBWSP->borderWidth( 1 );
                                        pInfoLeftBWSI->borderWidth( 1 );
                                        pInfoLeftBWSD->borderWidth( 1 );
                                        pInfoRightBWSP->borderWidth( 1 );
                                        pInfoRightBWSI->borderWidth( 1 );
                                        pInfoRightBWSD->borderWidth( 1 );
                                        pInfoLeftRefA->borderWidth( 1 );
                                        pInfoLeftRefB->borderWidth( 1 );
                                        pInfoLeftRefC->borderWidth( 1 );
                                        pInfoLeftRefD->borderWidth( 1 );
                                        pInfoRightRefA->borderWidth( 1 );
                                        pInfoRightRefB->borderWidth( 1 );
                                        pInfoRightRefC->borderWidth( 1 );
                                        pInfoRightRefD->borderWidth( 1 );
                                        return true;
                                    })                                
                                )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoRightWheelD )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 5;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 1 );
                                        pInfoLeftWheelI->borderWidth( 1 );
                                        pInfoLeftWheelD->borderWidth( 1 );
                                        pInfoRightWheelP->borderWidth( 1 );
                                        pInfoRightWheelI->borderWidth( 1 );
                                        pInfoRightWheelD->borderWidth( 3 );
                                        pInfoLeftBWSP->borderWidth( 1 );
                                        pInfoLeftBWSI->borderWidth( 1 );
                                        pInfoLeftBWSD->borderWidth( 1 );
                                        pInfoRightBWSP->borderWidth( 1 );
                                        pInfoRightBWSI->borderWidth( 1 );
                                        pInfoRightBWSD->borderWidth( 1 );
                                        pInfoLeftRefA->borderWidth( 1 );
                                        pInfoLeftRefB->borderWidth( 1 );
                                        pInfoLeftRefC->borderWidth( 1 );
                                        pInfoLeftRefD->borderWidth( 1 );
                                        pInfoRightRefA->borderWidth( 1 );
                                        pInfoRightRefB->borderWidth( 1 );
                                        pInfoRightRefC->borderWidth( 1 );
                                        pInfoRightRefD->borderWidth( 1 );
                                        return true;
                                    })                                
                                )
                            )
                        )
                        ._( Row() 
                            .size( -1, 0 )
                            .gap( 10 )
                            .border( false )
                            ._( Stack().size( 50, 0 ).border( false ) 
                                ._( Text( "BWS" ).align( ALIGN_VCENTERLEFT ))
                            )  
                            ._(Row()
                                .gap( 2 )
                                .border( false )
                                .size( -1, 0 )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoLeftBWSP )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 6;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 1 );
                                        pInfoLeftWheelI->borderWidth( 1 );
                                        pInfoLeftWheelD->borderWidth( 1 );
                                        pInfoRightWheelP->borderWidth( 1 );
                                        pInfoRightWheelI->borderWidth( 1 );
                                        pInfoRightWheelD->borderWidth( 1 );
                                        pInfoLeftBWSP->borderWidth( 3 );
                                        pInfoLeftBWSI->borderWidth( 1 );
                                        pInfoLeftBWSD->borderWidth( 1 );
                                        pInfoRightBWSP->borderWidth( 1 );
                                        pInfoRightBWSI->borderWidth( 1 );
                                        pInfoRightBWSD->borderWidth( 1 );
                                        pInfoLeftRefA->borderWidth( 1 );
                                        pInfoLeftRefB->borderWidth( 1 );
                                        pInfoLeftRefC->borderWidth( 1 );
                                        pInfoLeftRefD->borderWidth( 1 );
                                        pInfoRightRefA->borderWidth( 1 );
                                        pInfoRightRefB->borderWidth( 1 );
                                        pInfoRightRefC->borderWidth( 1 );
                                        pInfoRightRefD->borderWidth( 1 );
                                        return true;
                                    })                                
                                )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoLeftBWSI )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 7;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 1 );
                                        pInfoLeftWheelI->borderWidth( 1 );
                                        pInfoLeftWheelD->borderWidth( 1 );
                                        pInfoRightWheelP->borderWidth( 1 );
                                        pInfoRightWheelI->borderWidth( 1 );
                                        pInfoRightWheelD->borderWidth( 1 );
                                        pInfoLeftBWSP->borderWidth( 1 );
                                        pInfoLeftBWSI->borderWidth( 3 );
                                        pInfoLeftBWSD->borderWidth( 1 );
                                        pInfoRightBWSP->borderWidth( 1 );
                                        pInfoRightBWSI->borderWidth( 1 );
                                        pInfoRightBWSD->borderWidth( 1 );
                                        pInfoLeftRefA->borderWidth( 1 );
                                        pInfoLeftRefB->borderWidth( 1 );
                                        pInfoLeftRefC->borderWidth( 1 );
                                        pInfoLeftRefD->borderWidth( 1 );
                                        pInfoRightRefA->borderWidth( 1 );
                                        pInfoRightRefB->borderWidth( 1 );
                                        pInfoRightRefC->borderWidth( 1 );
                                        pInfoRightRefD->borderWidth( 1 );
                                        return true;
                                    })                                
                                )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoLeftBWSD )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 8;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 1 );
                                        pInfoLeftWheelI->borderWidth( 1 );
                                        pInfoLeftWheelD->borderWidth( 1 );
                                        pInfoRightWheelP->borderWidth( 1 );
                                        pInfoRightWheelI->borderWidth( 1 );
                                        pInfoRightWheelD->borderWidth( 1 );
                                        pInfoLeftBWSP->borderWidth( 1 );
                                        pInfoLeftBWSI->borderWidth( 1 );
                                        pInfoLeftBWSD->borderWidth( 3 );
                                        pInfoRightBWSP->borderWidth( 1 );
                                        pInfoRightBWSI->borderWidth( 1 );
                                        pInfoRightBWSD->borderWidth( 1 );
                                        pInfoLeftRefA->borderWidth( 1 );
                                        pInfoLeftRefB->borderWidth( 1 );
                                        pInfoLeftRefC->borderWidth( 1 );
                                        pInfoLeftRefD->borderWidth( 1 );
                                        pInfoRightRefA->borderWidth( 1 );
                                        pInfoRightRefB->borderWidth( 1 );
                                        pInfoRightRefC->borderWidth( 1 );
                                        pInfoRightRefD->borderWidth( 1 );
                                        return true;
                                    })                                
                                )
                            )
                            ._(Row()
                                .gap( 2 )
                                .border( false )
                                .size( -1, 0 )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoRightBWSP )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 9;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 1 );
                                        pInfoLeftWheelI->borderWidth( 1 );
                                        pInfoLeftWheelD->borderWidth( 1 );
                                        pInfoRightWheelP->borderWidth( 1 );
                                        pInfoRightWheelI->borderWidth( 1 );
                                        pInfoRightWheelD->borderWidth( 1 );
                                        pInfoLeftBWSP->borderWidth( 1 );
                                        pInfoLeftBWSI->borderWidth( 1 );
                                        pInfoLeftBWSD->borderWidth( 1 );
                                        pInfoRightBWSP->borderWidth( 3 );
                                        pInfoRightBWSI->borderWidth( 1 );
                                        pInfoRightBWSD->borderWidth( 1 );
                                        pInfoLeftRefA->borderWidth( 1 );
                                        pInfoLeftRefB->borderWidth( 1 );
                                        pInfoLeftRefC->borderWidth( 1 );
                                        pInfoLeftRefD->borderWidth( 1 );
                                        pInfoRightRefA->borderWidth( 1 );
                                        pInfoRightRefB->borderWidth( 1 );
                                        pInfoRightRefC->borderWidth( 1 );
                                        pInfoRightRefD->borderWidth( 1 );
                                        return true;
                                    })                                
                                )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoRightBWSI )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 10;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 1 );
                                        pInfoLeftWheelI->borderWidth( 1 );
                                        pInfoLeftWheelD->borderWidth( 1 );
                                        pInfoRightWheelP->borderWidth( 1 );
                                        pInfoRightWheelI->borderWidth( 1 );
                                        pInfoRightWheelD->borderWidth( 1 );
                                        pInfoLeftBWSP->borderWidth( 1 );
                                        pInfoLeftBWSI->borderWidth( 1 );
                                        pInfoLeftBWSD->borderWidth( 1 );
                                        pInfoRightBWSP->borderWidth( 1 );
                                        pInfoRightBWSI->borderWidth( 3 );
                                        pInfoRightBWSD->borderWidth( 1 );
                                        pInfoLeftRefA->borderWidth( 1 );
                                        pInfoLeftRefB->borderWidth( 1 );
                                        pInfoLeftRefC->borderWidth( 1 );
                                        pInfoLeftRefD->borderWidth( 1 );
                                        pInfoRightRefA->borderWidth( 1 );
                                        pInfoRightRefB->borderWidth( 1 );
                                        pInfoRightRefC->borderWidth( 1 );
                                        pInfoRightRefD->borderWidth( 1 );
                                        return true;
                                    })                                
                                )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoRightBWSD )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 11;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 1 );
                                        pInfoLeftWheelI->borderWidth( 1 );
                                        pInfoLeftWheelD->borderWidth( 1 );
                                        pInfoRightWheelP->borderWidth( 1 );
                                        pInfoRightWheelI->borderWidth( 1 );
                                        pInfoRightWheelD->borderWidth( 1 );
                                        pInfoLeftBWSP->borderWidth( 1 );
                                        pInfoLeftBWSI->borderWidth( 1 );
                                        pInfoLeftBWSD->borderWidth( 1 );
                                        pInfoRightBWSP->borderWidth( 1 );
                                        pInfoRightBWSI->borderWidth( 1 );
                                        pInfoRightBWSD->borderWidth( 3 );
                                        pInfoLeftRefA->borderWidth( 1 );
                                        pInfoLeftRefB->borderWidth( 1 );
                                        pInfoLeftRefC->borderWidth( 1 );
                                        pInfoLeftRefD->borderWidth( 1 );
                                        pInfoRightRefA->borderWidth( 1 );
                                        pInfoRightRefB->borderWidth( 1 );
                                        pInfoRightRefC->borderWidth( 1 );
                                        pInfoRightRefD->borderWidth( 1 );
                                        return true;
                                    })                                
                                )
                            )
                        )
                        ._( Row() 
                            .size( -1, 0 )
                            .gap( 10 )
                            .border( false )
                            ._( Stack().size( 50, 0 ).border( false ) 
                                ._( Text( "Ref." ).align( ALIGN_VCENTERLEFT ))
                            )  
                            ._(Row()
                                .gap( 2 )
                                .border( false )
                                .size( -1, 0 )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoLeftRefA )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 12;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 1 );
                                        pInfoLeftWheelI->borderWidth( 1 );
                                        pInfoLeftWheelD->borderWidth( 1 );
                                        pInfoRightWheelP->borderWidth( 1 );
                                        pInfoRightWheelI->borderWidth( 1 );
                                        pInfoRightWheelD->borderWidth( 1 );
                                        pInfoLeftBWSP->borderWidth( 1 );
                                        pInfoLeftBWSI->borderWidth( 1 );
                                        pInfoLeftBWSD->borderWidth( 1 );
                                        pInfoRightBWSP->borderWidth( 1 );
                                        pInfoRightBWSI->borderWidth( 1 );
                                        pInfoRightBWSD->borderWidth( 1 );
                                        pInfoLeftRefA->borderWidth( 3 );
                                        pInfoLeftRefB->borderWidth( 1 );
                                        pInfoLeftRefC->borderWidth( 1 );
                                        pInfoLeftRefD->borderWidth( 1 );
                                        pInfoRightRefA->borderWidth( 1 );
                                        pInfoRightRefB->borderWidth( 1 );
                                        pInfoRightRefC->borderWidth( 1 );
                                        pInfoRightRefD->borderWidth( 1 );
                                        return true;
                                    })                                
                                )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoLeftRefB )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 13;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 1 );
                                        pInfoLeftWheelI->borderWidth( 1 );
                                        pInfoLeftWheelD->borderWidth( 1 );
                                        pInfoRightWheelP->borderWidth( 1 );
                                        pInfoRightWheelI->borderWidth( 1 );
                                        pInfoRightWheelD->borderWidth( 1 );
                                        pInfoLeftBWSP->borderWidth( 1 );
                                        pInfoLeftBWSI->borderWidth( 1 );
                                        pInfoLeftBWSD->borderWidth( 1 );
                                        pInfoRightBWSP->borderWidth( 1 );
                                        pInfoRightBWSI->borderWidth( 1 );
                                        pInfoRightBWSD->borderWidth( 1 );
                                        pInfoLeftRefA->borderWidth( 1 );
                                        pInfoLeftRefB->borderWidth( 3 );
                                        pInfoLeftRefC->borderWidth( 1 );
                                        pInfoLeftRefD->borderWidth( 1 );
                                        pInfoRightRefA->borderWidth( 1 );
                                        pInfoRightRefB->borderWidth( 1 );
                                        pInfoRightRefC->borderWidth( 1 );
                                        pInfoRightRefD->borderWidth( 1 );
                                        return true;
                                    })                                
                                )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoLeftRefC )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 14;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 1 );
                                        pInfoLeftWheelI->borderWidth( 1 );
                                        pInfoLeftWheelD->borderWidth( 1 );
                                        pInfoRightWheelP->borderWidth( 1 );
                                        pInfoRightWheelI->borderWidth( 1 );
                                        pInfoRightWheelD->borderWidth( 1 );
                                        pInfoLeftBWSP->borderWidth( 1 );
                                        pInfoLeftBWSI->borderWidth( 1 );
                                        pInfoLeftBWSD->borderWidth( 1 );
                                        pInfoRightBWSP->borderWidth( 1 );
                                        pInfoRightBWSI->borderWidth( 1 );
                                        pInfoRightBWSD->borderWidth( 1 );
                                        pInfoLeftRefA->borderWidth( 1 );
                                        pInfoLeftRefB->borderWidth( 1 );
                                        pInfoLeftRefC->borderWidth( 3 );
                                        pInfoLeftRefD->borderWidth( 1 );
                                        pInfoRightRefA->borderWidth( 1 );
                                        pInfoRightRefB->borderWidth( 1 );
                                        pInfoRightRefC->borderWidth( 1 );
                                        pInfoRightRefD->borderWidth( 1 );
                                        return true;
                                    })                                
                                )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoLeftRefD )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 15;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 1 );
                                        pInfoLeftWheelI->borderWidth( 1 );
                                        pInfoLeftWheelD->borderWidth( 1 );
                                        pInfoRightWheelP->borderWidth( 1 );
                                        pInfoRightWheelI->borderWidth( 1 );
                                        pInfoRightWheelD->borderWidth( 1 );
                                        pInfoLeftBWSP->borderWidth( 1 );
                                        pInfoLeftBWSI->borderWidth( 1 );
                                        pInfoLeftBWSD->borderWidth( 1 );
                                        pInfoRightBWSP->borderWidth( 1 );
                                        pInfoRightBWSI->borderWidth( 1 );
                                        pInfoRightBWSD->borderWidth( 1 );
                                        pInfoLeftRefA->borderWidth( 1 );
                                        pInfoLeftRefB->borderWidth( 1 );
                                        pInfoLeftRefC->borderWidth( 1 );
                                        pInfoLeftRefD->borderWidth( 3 );
                                        pInfoRightRefA->borderWidth( 1 );
                                        pInfoRightRefB->borderWidth( 1 );
                                        pInfoRightRefC->borderWidth( 1 );
                                        pInfoRightRefD->borderWidth( 1 );
                                        return true;
                                    })                                
                                )
                            )
                            ._(Row()
                                .gap( 2 )
                                .border( false )
                                .size( -1, 0 )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoRightRefA )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 16;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 1 );
                                        pInfoLeftWheelI->borderWidth( 1 );
                                        pInfoLeftWheelD->borderWidth( 1 );
                                        pInfoRightWheelP->borderWidth( 1 );
                                        pInfoRightWheelI->borderWidth( 1 );
                                        pInfoRightWheelD->borderWidth( 1 );
                                        pInfoLeftBWSP->borderWidth( 1 );
                                        pInfoLeftBWSI->borderWidth( 1 );
                                        pInfoLeftBWSD->borderWidth( 1 );
                                        pInfoRightBWSP->borderWidth( 1 );
                                        pInfoRightBWSI->borderWidth( 1 );
                                        pInfoRightBWSD->borderWidth( 1 );
                                        pInfoLeftRefA->borderWidth( 1 );
                                        pInfoLeftRefB->borderWidth( 1 );
                                        pInfoLeftRefC->borderWidth( 1 );
                                        pInfoLeftRefD->borderWidth( 1 );
                                        pInfoRightRefA->borderWidth( 3 );
                                        pInfoRightRefB->borderWidth( 1 );
                                        pInfoRightRefC->borderWidth( 1 );
                                        pInfoRightRefD->borderWidth( 1 );
                                        return true;
                                    })                                
                                )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoRightRefB )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 17;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 1 );
                                        pInfoLeftWheelI->borderWidth( 1 );
                                        pInfoLeftWheelD->borderWidth( 1 );
                                        pInfoRightWheelP->borderWidth( 1 );
                                        pInfoRightWheelI->borderWidth( 1 );
                                        pInfoRightWheelD->borderWidth( 1 );
                                        pInfoLeftBWSP->borderWidth( 1 );
                                        pInfoLeftBWSI->borderWidth( 1 );
                                        pInfoLeftBWSD->borderWidth( 1 );
                                        pInfoRightBWSP->borderWidth( 1 );
                                        pInfoRightBWSI->borderWidth( 1 );
                                        pInfoRightBWSD->borderWidth( 1 );
                                        pInfoLeftRefA->borderWidth( 1 );
                                        pInfoLeftRefB->borderWidth( 1 );
                                        pInfoLeftRefC->borderWidth( 1 );
                                        pInfoLeftRefD->borderWidth( 1 );
                                        pInfoRightRefA->borderWidth( 1 );
                                        pInfoRightRefB->borderWidth( 3 );
                                        pInfoRightRefC->borderWidth( 1 );
                                        pInfoRightRefD->borderWidth( 1 );
                                        return true;
                                    })                                
                                )
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoRightRefC )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 18;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 1 );
                                        pInfoLeftWheelI->borderWidth( 1 );
                                        pInfoLeftWheelD->borderWidth( 1 );
                                        pInfoRightWheelP->borderWidth( 1 );
                                        pInfoRightWheelI->borderWidth( 1 );
                                        pInfoRightWheelD->borderWidth( 1 );
                                        pInfoLeftBWSP->borderWidth( 1 );
                                        pInfoLeftBWSI->borderWidth( 1 );
                                        pInfoLeftBWSD->borderWidth( 1 );
                                        pInfoRightBWSP->borderWidth( 1 );
                                        pInfoRightBWSI->borderWidth( 1 );
                                        pInfoRightBWSD->borderWidth( 1 );
                                        pInfoLeftRefA->borderWidth( 1 );
                                        pInfoLeftRefB->borderWidth( 1 );
                                        pInfoLeftRefC->borderWidth( 1 );
                                        pInfoLeftRefD->borderWidth( 1 );
                                        pInfoRightRefA->borderWidth( 1 );
                                        pInfoRightRefB->borderWidth( 1 );
                                        pInfoRightRefC->borderWidth( 3 );
                                        pInfoRightRefD->borderWidth( 1 );
                                        return true;
                                    })                                
                                )                                
                                ._( BWSInfoBox() 
                                    .ref( (DecWidget **)&pInfoRightRefD )
                                    .border_color( 0xFFf5ab2b )
                                    .size( -1, 24 )
                                    .onMousePressed( [=]( DecWidget &w ) -> bool {
                                        nPIDMode = 19;
                                        YellowSetUIValues();
                                        YellowGetKeyPadString();
                                        pInfoLeftLeverSpeed->borderWidth( 1 );
                                        pInfoRightLeverSpeed->borderWidth( 1 );
                                        pInfoLeftFallingLimit->borderWidth( 1 );
                                        pInfoRightFallingLimit->borderWidth( 1 );
                                        pInfoLeftWheelP->borderWidth( 1 );
                                        pInfoLeftWheelI->borderWidth( 1 );
                                        pInfoLeftWheelD->borderWidth( 1 );
                                        pInfoRightWheelP->borderWidth( 1 );
                                        pInfoRightWheelI->borderWidth( 1 );
                                        pInfoRightWheelD->borderWidth( 1 );
                                        pInfoLeftBWSP->borderWidth( 1 );
                                        pInfoLeftBWSI->borderWidth( 1 );
                                        pInfoLeftBWSD->borderWidth( 1 );
                                        pInfoRightBWSP->borderWidth( 1 );
                                        pInfoRightBWSI->borderWidth( 1 );
                                        pInfoRightBWSD->borderWidth( 1 );
                                        pInfoLeftRefA->borderWidth( 1 );
                                        pInfoLeftRefB->borderWidth( 1 );
                                        pInfoLeftRefC->borderWidth( 1 );
                                        pInfoLeftRefD->borderWidth( 1 );
                                        pInfoRightRefA->borderWidth( 1 );
                                        pInfoRightRefB->borderWidth( 1 );
                                        pInfoRightRefC->borderWidth( 1 );
                                        pInfoRightRefD->borderWidth( 3 );
                                        return true;
                                    })                                
                                )
                            )
                        )
                    )
                )
                ._( Row()
                    .size( -1, -1 )
                    .gap( 10 )
                    .border( false )
                    ._( Column()
                        .ref( (DecWidget **)&_pFallingLimitMode )
                        .size( 0, -1 )
                        .border( true )
                        .border_color( 0xFFf5ab2b )
                        .borderWidth( 2 )
                        .padding( 10, 10, 10, 10 )
                        .background( false )
                        .onMousePressed( [=]( DecWidget &widget ) -> bool {
                            SkDebugf( "Mode Falling Limit (Yellow 1)\n" );
                            nYelloMode = 1;
                            _pLeverSpeedMode->borderWidth( 2 );
                            _pFallingLimitMode->borderWidth( 4 );
                            _pPIDMode->borderWidth( 2 );
                            return false;
                        })                        
                        ._( Stack().size( 150, 0 ).border( false ) 
                            ._( Text( "Falling Limit" ).align( ALIGN_CENTER ))
                        )  
                        ._( Spacer().size( 0, -1 ) )
                        ._( Row()
                            .size( 170, 0 )
                            .gap( 10 )
                            .border( false )
                            ._( Stack().size( -1, 0 ).border( false ) 
                                ._( Text( "Left" ).align( ALIGN_CENTER ))
                            ) 
                            ._( Stack().size( -1, 0 ).border( false ) 
                                ._( Text( "Right" ).align( ALIGN_CENTER ))
                            ) 
                        )                        
                        ._( Row()
                            .size( 0, 0 )
                            .gap( 10 )
                            .border( false )
                            ._( BWSInfoBox() 
                                .ref( (DecWidget **)&pInfoLeftFallingLimit)
                                .size( 80, 44 )
                                .border_color( 0xFFf5ab2b )
                                .onMousePressed( [=]( DecWidget &w ) -> bool {
                                    nFallingLimitMode = 0;
                                    YellowSetUIValues();
                                    YellowGetKeyPadString();
                                    pInfoLeftLeverSpeed->borderWidth( 1 );
                                    pInfoRightLeverSpeed->borderWidth( 1 );
                                    pInfoLeftFallingLimit->borderWidth( 3 );
                                    pInfoRightFallingLimit->borderWidth( 1 );
                                    pInfoLeftWheelP->borderWidth( 1 );
                                    pInfoLeftWheelI->borderWidth( 1 );
                                    pInfoLeftWheelD->borderWidth( 1 );
                                    pInfoRightWheelP->borderWidth( 1 );
                                    pInfoRightWheelI->borderWidth( 1 );
                                    pInfoRightWheelD->borderWidth( 1 );
                                    pInfoLeftBWSP->borderWidth( 1 );
                                    pInfoLeftBWSI->borderWidth( 1 );
                                    pInfoLeftBWSD->borderWidth( 1 );
                                    pInfoRightBWSP->borderWidth( 1 );
                                    pInfoRightBWSI->borderWidth( 1 );
                                    pInfoRightBWSD->borderWidth( 1 );
                                    pInfoLeftRefA->borderWidth( 1 );
                                    pInfoLeftRefB->borderWidth( 1 );
                                    pInfoLeftRefC->borderWidth( 1 );
                                    pInfoLeftRefD->borderWidth( 1 );
                                    pInfoRightRefA->borderWidth( 1 );
                                    pInfoRightRefB->borderWidth( 1 );
                                    pInfoRightRefC->borderWidth( 1 );
                                    pInfoRightRefD->borderWidth( 1 );                               
                                    return true;
                            })                                

                            )
                            ._( BWSInfoBox() 
                                .ref( (DecWidget **)&pInfoRightFallingLimit)
                                .size( 80, 44 )
                                .border_color( 0xFFf5ab2b )
                                .onMousePressed( [=]( DecWidget &w ) -> bool {
                                    nFallingLimitMode = 1;
                                    YellowSetUIValues();
                                    YellowGetKeyPadString();
                                    pInfoLeftLeverSpeed->borderWidth( 1 );
                                    pInfoRightLeverSpeed->borderWidth( 1 );
                                    pInfoLeftFallingLimit->borderWidth( 1 );
                                    pInfoRightFallingLimit->borderWidth( 3 );
                                    pInfoLeftWheelP->borderWidth( 1 );
                                    pInfoLeftWheelI->borderWidth( 1 );
                                    pInfoLeftWheelD->borderWidth( 1 );
                                    pInfoRightWheelP->borderWidth( 1 );
                                    pInfoRightWheelI->borderWidth( 1 );
                                    pInfoRightWheelD->borderWidth( 1 );
                                    pInfoLeftBWSP->borderWidth( 1 );
                                    pInfoLeftBWSI->borderWidth( 1 );
                                    pInfoLeftBWSD->borderWidth( 1 );
                                    pInfoRightBWSP->borderWidth( 1 );
                                    pInfoRightBWSI->borderWidth( 1 );
                                    pInfoRightBWSD->borderWidth( 1 );
                                    pInfoLeftRefA->borderWidth( 1 );
                                    pInfoLeftRefB->borderWidth( 1 );
                                    pInfoLeftRefC->borderWidth( 1 );
                                    pInfoLeftRefD->borderWidth( 1 );
                                    pInfoRightRefA->borderWidth( 1 );
                                    pInfoRightRefB->borderWidth( 1 );
                                    pInfoRightRefC->borderWidth( 1 );
                                    pInfoRightRefD->borderWidth( 1 );                                   
                                    return true;
                                })                                
                            )
                        )
                        ._( Spacer().size( 0, -1 ) )
                    )      
                    ._( Row()
                        .size( -1, -1 )
                        .border( true )
                        .border_color( 0xFFf5ab2b )
                        .borderWidth( 2 )
                        .padding( 7, 10, 5, 10 )
                        .background( false )
                        .gap( 10 )
                        ._( Spacer().size( -1, 0 ) )
                        ._( Column()
                            .ref( (DecWidget **)&pKeyPad )
                            .size( 0, 0 )
                            .align( ALIGN_TOPLEFT )
                            .round( 10 )
                            .padding( 10, 10, 10, 10 )
                            .border_color( 0x80808080 )
                            .gap( 3 )
                            ._( Row()
                                .gap( 10 )
                                .size( 0, 0 )
                                .border( false )
                                ._( Button( "1", 0, [=]( DecWidget &w) -> bool {
                                    KeyPadAdd( "1" );
                                    return true;
                                }).size( 32, 32 ).border_color( 0x80808080 ) )
                                ._( Button( "2", 0, [=]( DecWidget &w) -> bool {
                                    KeyPadAdd( "2" );
                                    return true;
                                }).size( 32, 32 ).border_color( 0x80808080 ) )
                                ._( Button( "3", 0, [=]( DecWidget &w) -> bool {
                                    KeyPadAdd( "3" );
                                    return true;
                                }).size( 32, 32 ).border_color( 0x80808080 ) )
                            )
                            ._( Row()
                                .gap( 10 )
                                .size( 0, 0 )
                                .border( false )
                                ._( Button( "4", 0, [=]( DecWidget &w) -> bool {
                                    KeyPadAdd( "4" );
                                    return true;
                                }).size( 32, 32 ).border_color( 0x80808080 ) )
                                ._( Button( "5", 0, [=]( DecWidget &w) -> bool {
                                    KeyPadAdd( "5" );
                                    return true;
                                }).size( 32, 32 ).border_color( 0x80808080 ) )
                                ._( Button( "6", 0, [=]( DecWidget &w) -> bool {
                                    KeyPadAdd( "6" );
                                    return true;
                                }).size( 32, 32 ).border_color( 0x80808080 ) )
                            )            
                            ._( Row()
                                .size( 0, 0 )
                                .gap( 10 )
                                .border( false )
                                ._( Button( "7", 0, [=]( DecWidget &w) -> bool {
                                    KeyPadAdd( "7" );
                                    return true;
                                } ).size( 32, 32 ).border_color( 0x80808080 ) )
                                ._( Button( "8", 0, [=]( DecWidget &w) -> bool {
                                    KeyPadAdd( "8" );
                                    return true;
                                }).size( 32, 32 ).border_color( 0x80808080 ) )
                                ._( Button( "9", 0, [=]( DecWidget &w) -> bool {
                                    KeyPadAdd( "9" );
                                    return true;
                                }).size( 32, 32 ).border_color( 0x80808080 ) )
                            )        
                            ._( Row()
                                .size( 0, 0 )
                                .gap( 10 )
                                .border( false )
                                ._( Button( "", 0xf55a, [=]( DecWidget &w) -> bool {
                                    //addSearchChar( "X" );
                                    KeyPadAdd("X");
                                    return true;
                                }).size( 32, 32 ).border_color( 0x80808080 ) )
                                ._( Button( "0", 0, [=]( DecWidget &w) -> bool {
                                    KeyPadAdd( "0" );
                                    return true;
                                }).size( 32, 32 ).border_color( 0x80808080 ) )
                                ._( Button( ".", 0, [=]( DecWidget &w) -> bool {
                                    KeyPadAdd( "." );
                                    return true;
                                }).size( 32, 32 ).border_color( 0x80808080 ) )
                            )        
                        )
                        ._( Column()
                            .size( 0, -1 )
                            .border( false )
                            .gap( 10 )
                            ._( Spacer().size( 0, -1 ) )
                            ._( Button( "Clear", 0, [=]( DecWidget &w) -> bool {
                                    ParamClear();
                                    return true;
                                })
                                .size( 80, 30 )
                                .border_color( 0x80808080 ) 
                                .align( ALIGN_VCENTERLEFT )
                            )
                            ._( Button( "Send", 0, [=]( DecWidget &w) -> bool {
                                    if( bR2 ) {
                                        KeyPadSetValue();
                                    }
                                    return true;
                                })
                                .size( 80, 80 )
                                .border_color( 0x80808080 ) 
                                .align( ALIGN_VCENTERLEFT )
                            )
                            ._( Spacer().size( 0, -1 ) )
                        )
                        ._( Spacer().size( -1, 0 ) )
                    )                                    
                )
            )
        )
    );
}

DecBWSUI::~DecBWSUI(){

} 

unsigned int DecBWSUI::hex2int( const char c ) {
    if( c >= '0' && c <= '9' ) {
        return c - '0';
    }
    else if( c >= 'A' && c <= 'F' ) {
        return c - 'A' + 10;
    }
    else if( c >= 'a' && c <= 'f' ) {
        return c - 'a' + 10;
    }    
    return 0;
}

int DecBWSUI::parseInt( std::string str, int index ) {
    int n1 = parseByte( str, index );
    int n2 = parseByte( str, index+1 );
    SkDebugf( "Int: %i %i\n", n1, n2 );
    int n = n1 + (n2*256);
    return n;
}

unsigned int DecBWSUI::parseByte( std::string str, int index ) {
    int i = index * 2;
    int n1 = (hex2int(str[i]));
    int n2 = (hex2int(str[i+1]));
    unsigned int n = n2 + (n1*16);
    return n;
}

float DecBWSUI::parseFloat( std::string str, int index ) {
    unsigned char b[4];
    b[0] = parseByte( str, index );
    b[1] = parseByte( str, index+1 );
    b[2] = parseByte( str, index+2 );
    b[3] = parseByte( str, index+3 );
    float f = *((float *)b);
    return f;
}

void DecBWSUI::js_callback( std::string message ) {
    SkDebugf( "BWS Callback: %s\n", message.c_str() );
    if( message.length() < 316 ) {
        return;
    }
    int nMode = parseInt( message, 0 );
    if( nMode == 1 ) {
        isTraining = true;
        _pButtonStartTraining->text_color( 0xFF2f9b34 );
        _pButtonStopTraining->text_color( 0xFFC0C0C0 );
    }
    else if( nMode == 0 ) {                                                                        
        isTraining = false;
        _pButtonStartTraining->text_color( 0xFFC0C0C0 );
        _pButtonStopTraining->text_color( 0xFF2f9b34 );        
    }

    float lever_l = parseFloat( message, 2 );
    float lever_r = parseFloat( message, 6 );
    SkDebugf( "Mode: %i\n", nMode );
    SkDebugf( "Lever: %0.2f %0.2f\n", lever_l, lever_r );

    fLeftLeverSpeed = fRightLeverSpeed = parseFloat( message, 18 );
    SkDebugf( "Lever Speed: %0.2f %0.2f\n", fLeftLeverSpeed, fRightLeverSpeed );

    fLeftWeight = parseFloat( message, 22 );
    fRightWeight = parseFloat( message, 26 );
    SkDebugf( "Weight Support: %0.2f %0.2f\n", fLeftWeight, fRightWeight );

    fLeftFallingLimit = parseFloat( message, 30 );
    fRightFallingLimit = parseFloat( message, 34 );
    SkDebugf( "Falling Limit: %0.2f %0.2f\n", fLeftFallingLimit, fRightFallingLimit );

    fMaxSpeed = parseFloat( message, 48 );
    SkDebugf( "Speed Limit: %0.2f\n", fMaxSpeed );

    fPID[0] = parseFloat( message, 74 );
    fPID[1] = parseFloat( message, 78 );
    fPID[2] = parseFloat( message, 82 );
    fPID[3] = parseFloat( message, 86 );
    fPID[4] = parseFloat( message, 90 );
    fPID[5] = parseFloat( message, 94 );
    SkDebugf( "WheelLeft: %0.2f %0.2f %0.2f\n", fPID[0], fPID[1], fPID[2] );
    SkDebugf( "WheelRight: %0.2f %0.2f %0.2f\n", fPID[3], fPID[4], fPID[5] );

    fPID[6] = parseFloat( message, 98 );
    fPID[7] = parseFloat( message, 102 );
    fPID[8] = parseFloat( message, 106 );
    fPID[9] = parseFloat( message, 110 );
    fPID[10] = parseFloat( message, 114 );
    fPID[11] = parseFloat( message, 118 );
    SkDebugf( "BWSLeft: %0.2f %0.2f %0.2f\n", fPID[6], fPID[7], fPID[8] );
    SkDebugf( "BWSRight: %0.2f %0.2f %0.2f\n", fPID[9], fPID[10], fPID[11] );   

    fPID[12] = parseFloat( message, 122 );
    fPID[13] = parseFloat( message, 126 );
    fPID[14] = parseFloat( message, 130 );
    fPID[15] = parseFloat( message, 134 );
    fPID[16] = parseFloat( message, 138 );
    fPID[17] = parseFloat( message, 142 );
    fPID[18] = parseFloat( message, 146 );
    fPID[19] = parseFloat( message, 150 );
    SkDebugf( "RefLeft: %0.2f %0.2f %0.2f %0.2f\n", fPID[12], fPID[13], fPID[14], fPID[15] );
    SkDebugf( "RefRight: %0.2f %0.2f %0.2f %0.2f\n", fPID[16], fPID[17], fPID[18], fPID[19] );

    YellowSetUIValues();
}   

void DecBWSUI::init() {
    SkDebugf( "DecBWSUI::init\n" );
#ifdef __EMSCRIPTEN__
    char szx[512];
    sprintf(szx, "skappSetCallback('%lu')", (unsigned long)this);
    emscripten_run_script(szx);
#endif    

    pLeftWeightSupportButton->bg_color( 0xFFf74648 );
    pRightWeightSupportButton->bg_color( 0xFFf74648 );

    char sz[32];
    sprintf( sz, "%0.2f m/s", fLeftLeverSpeed );
    pInfoLeverSpeed->text( sz );

    sprintf( sz, "L %0.1f kg", fLeftWeight );
    pLeftWeightSupportText->text( sz );
    pLeftWeightSupportText2->text( sz );
    sprintf( sz, "R %0.1f kg", fRightWeight );
    pRightWeightSupportText->text( sz );
    pRightWeightSupportText2->text( sz );

    if( nTrainingMode == 0 ) {
        sprintf( sz, "%0.2f m/s", fMaxSpeed );
        pTrainingMaxSpeedBox->text( sz );
        pTrainingMaxSpeedBox2->text( sz );
        _pTextModeName->text( "Straight");
    }
    else if( nTrainingMode == 1 ) {
        sprintf( sz, "%0.2f m/s", fMaxSpeed );
        pTrainingMaxSpeedBox->text( sz );
        _pTextModeName->text( "Following");
        pTrainingMaxSpeedBox2->text( sz );
    }
    else if( nTrainingMode == 2 ) {
        sprintf( sz, "%0.2f m/s", fMaxSpeed );
        pTrainingMaxSpeedBox->text( sz );                        
        _pTextModeName->text( "Manual");
        pTrainingMaxSpeedBox2->text( sz );
    }    

    sprintf( sz, "%0.1f cm", fLeftPos );
    pInfoLeftPos->text( sz );
    sprintf( sz, "%0.1f cm", fRightPos );
    pInfoRightPos->text( sz );    

    // Yellow Init
    YellowSetUIValues();   

    _pButtonStartTraining->invalidate();
    _pButtonStopTraining->invalidate();
    _pButtonStartTraining->dirty( true, true );
    _pButtonStopTraining->dirty( true, true );

    
}

DecBWSUI &BWSUI() {
    DecBWSUI *w = new DecBWSUI();
    return *w;
}

void DecBWSUI::YellowSetUIValues() {
    char sz[128];

    // Red
    sprintf( sz, "%0.2f m/s", fLeftLeverSpeed );
    pInfoLeverSpeed->text( sz );

    sprintf( sz, "L %0.1f kg", fLeftWeight );
    pLeftWeightSupportText->text( sz );
    pLeftWeightSupportText2->text( sz );
    sprintf( sz, "R %0.1f kg", fRightWeight );
    pRightWeightSupportText->text( sz );
    pRightWeightSupportText2->text( sz );   

    sprintf( sz, "%0.2f m/s", fMaxSpeed );
    pTrainingMaxSpeedBox->text( sz );
    pTrainingMaxSpeedBox2->text( sz );    

    // Yellow
    sprintf( sz, "%0.2f", fLeftLeverSpeed );
    pInfoLeftLeverSpeed->text( sz );
    sprintf( sz, "%0.2f", fRightLeverSpeed );
    pInfoRightLeverSpeed->text( sz );

    sprintf( sz, "%0.2f", fLeftFallingLimit );
    pInfoLeftFallingLimit->text( sz );
    sprintf( sz, "%0.2f", fRightFallingLimit );
    pInfoRightFallingLimit->text( sz );

    sprintf( sz, "%0.2f", fPID[0] );
    pInfoLeftWheelP->text( sz );
    sprintf( sz, "%0.2f", fPID[1] );
    pInfoLeftWheelI->text( sz );
    sprintf( sz, "%0.2f", fPID[2] );
    pInfoLeftWheelD->text( sz );

    sprintf( sz, "%0.2f", fPID[3] );
    pInfoRightWheelP->text( sz );
    sprintf( sz, "%0.2f", fPID[4] );
    pInfoRightWheelI->text( sz );
    sprintf( sz, "%0.2f", fPID[5] );
    pInfoRightWheelD->text( sz );

    sprintf( sz, "%0.2f", fPID[6] );
    pInfoLeftBWSP->text( sz );
    sprintf( sz, "%0.2f", fPID[7] );
    pInfoLeftBWSI->text( sz );
    sprintf( sz, "%0.2f", fPID[8] );
    pInfoLeftBWSD->text( sz );

    sprintf( sz, "%0.2f", fPID[9] );
    pInfoRightBWSP->text( sz );
    sprintf( sz, "%0.2f", fPID[10] );
    pInfoRightBWSI->text( sz );
    sprintf( sz, "%0.2f", fPID[11] );
    pInfoRightBWSD->text( sz );    

    sprintf( sz, "%0.2f", fPID[12] );
    pInfoLeftRefA->text( sz );
    sprintf( sz, "%0.2f", fPID[13] );
    pInfoLeftRefB->text( sz );
    sprintf( sz, "%0.2f", fPID[14] );
    pInfoLeftRefC->text( sz );
    sprintf( sz, "%0.2f", fPID[15] );
    pInfoLeftRefD->text( sz );

    sprintf( sz, "%0.2f", fPID[16] );
    pInfoRightRefA->text( sz );
    sprintf( sz, "%0.2f", fPID[17] );
    pInfoRightRefB->text( sz );
    sprintf( sz, "%0.2f", fPID[18] );
    pInfoRightRefC->text( sz );  
    sprintf( sz, "%0.2f", fPID[19] );
    pInfoRightRefD->text( sz );
}

void DecBWSUI::KeyPadAdd( const char *ch ) {
    if( ch[0] == 'X' ) {
        KeyPadText = KeyPadText.substr( 0, KeyPadText.length()-1 );
    }
    else {
        KeyPadText = KeyPadText + std::string( ch );
    }
    YellowSetKeyPadString();
}

void DecBWSUI::YellowSetKeyPadString() {
    if( nYelloMode == 0 ) {
        if( nLeverSpeedMode == 0 ) {
            pInfoLeftLeverSpeed->text( KeyPadText.c_str() );
        }
        if( nLeverSpeedMode == 1 ) {
            pInfoRightLeverSpeed->text( KeyPadText.c_str() );
        }
    }
    if( nYelloMode == 1 ) {
        if( nFallingLimitMode == 0 ) {
            pInfoLeftFallingLimit->text( KeyPadText.c_str() );
        }
        if( nFallingLimitMode == 1 ) {
            pInfoRightFallingLimit->text( KeyPadText.c_str() );
        }
    }
    if( nYelloMode == 2 ) {
        if( nPIDMode == 0 ) {
            pInfoLeftWheelP->text( KeyPadText.c_str() );
        }
        else if( nPIDMode == 1 ) {
            pInfoLeftWheelI->text( KeyPadText.c_str() );
        }
        else if( nPIDMode == 2 ) {
            pInfoLeftWheelD->text( KeyPadText.c_str() );
        }
        else if( nPIDMode == 3 ) {
            pInfoRightWheelP->text( KeyPadText.c_str() );
        }
        else if( nPIDMode == 4 ) {
            pInfoRightWheelI->text( KeyPadText.c_str() );
        }
        else if( nPIDMode == 5 ) {
            pInfoRightWheelD->text( KeyPadText.c_str() );
        }
        else if( nPIDMode == 6 ) {
            pInfoLeftBWSP->text( KeyPadText.c_str() );
        }
        else if( nPIDMode == 7 ) {
            pInfoLeftBWSI->text( KeyPadText.c_str() );
        }
        else if( nPIDMode == 8 ) {
            pInfoLeftBWSD->text( KeyPadText.c_str() );
        }
        else if( nPIDMode == 9 ) {
            pInfoRightBWSP->text( KeyPadText.c_str() );
        }
        else if( nPIDMode == 10 ) {
            pInfoRightBWSI->text( KeyPadText.c_str() );
        }
        else if( nPIDMode == 11 ) {
            pInfoRightBWSD->text( KeyPadText.c_str() );
        }        
        else if( nPIDMode == 12 ) {
            pInfoLeftRefA->text( KeyPadText.c_str() );
        }
        else if( nPIDMode == 13 ) {
            pInfoLeftRefB->text( KeyPadText.c_str() );
        }
        else if( nPIDMode == 14 ) {
            pInfoLeftRefC->text( KeyPadText.c_str() );
        }
        else if( nPIDMode == 15 ) {
            pInfoLeftRefD->text( KeyPadText.c_str() );
        }
        else if( nPIDMode == 16 ) {
            pInfoRightRefA->text( KeyPadText.c_str() );
        }
        else if( nPIDMode == 17 ) {
            pInfoRightRefB->text( KeyPadText.c_str() );
        }        
        else if( nPIDMode == 18 ) {
            pInfoRightRefC->text( KeyPadText.c_str() );
        }        
        else if( nPIDMode == 19 ) {
            pInfoRightRefD->text( KeyPadText.c_str() );
        }        
    }
}

void DecBWSUI::KeyPadSetValue() {
    if( !bR2 ) {
        return;
    }
    
    if( nYelloMode == 0 ) {
        if( nLeverSpeedMode == 0 ) {
            fLeftLeverSpeed = atof( KeyPadText.c_str() );
        }
        if( nLeverSpeedMode == 1 ) {
            fRightLeverSpeed = atof( KeyPadText.c_str() );
        }
    }
    if( nYelloMode == 1 ) {
        if( nFallingLimitMode == 0 ) {
            fLeftFallingLimit = atof( KeyPadText.c_str() );
        }
        if( nFallingLimitMode == 1 ) {
            fRightFallingLimit = atof( KeyPadText.c_str() );
        }
    }
    if( nYelloMode == 2 ) {
        if( nPIDMode == 0 ) {
            fPID[0] = atof( KeyPadText.c_str() );
        }
        else if( nPIDMode == 1 ) {
            fPID[1] = atof( KeyPadText.c_str() );
        }
        else if( nPIDMode == 2 ) {
            fPID[2] = atof( KeyPadText.c_str() );
        }
        else if( nPIDMode == 3 ) {
            fPID[3] = atof( KeyPadText.c_str() );
        }
        else if( nPIDMode == 4 ) {
            fPID[4] = atof( KeyPadText.c_str() );
        }
        else if( nPIDMode == 5 ) {
            fPID[5] = atof( KeyPadText.c_str() );
        }
        else if( nPIDMode == 6 ) {
            fPID[6] = atof( KeyPadText.c_str() );
        }
        else if( nPIDMode == 7 ) {
            fPID[7] = atof( KeyPadText.c_str() );
        }
        else if( nPIDMode == 8 ) {
            fPID[8] = atof( KeyPadText.c_str() );
        }
        else if( nPIDMode == 9 ) {
            fPID[9] = atof( KeyPadText.c_str() );
        }
        else if( nPIDMode == 10 ) {
            fPID[10] = atof( KeyPadText.c_str() );
        }
        else if( nPIDMode == 11 ) {
            fPID[11] = atof( KeyPadText.c_str() );
        }        
        else if( nPIDMode == 12 ) {
            fPID[12] = atof( KeyPadText.c_str() );
        }
        else if( nPIDMode == 13 ) {
            fPID[13] = atof( KeyPadText.c_str() );
        }
        else if( nPIDMode == 14 ) {
            fPID[14] = atof( KeyPadText.c_str() );
        }
        else if( nPIDMode == 15 ) {
            fPID[15] = atof( KeyPadText.c_str() );
        }
        else if( nPIDMode == 16 ) {
            fPID[16] = atof( KeyPadText.c_str() );
        }
        else if( nPIDMode == 17 ) {
            fPID[17] = atof( KeyPadText.c_str() );
        }        
        else if( nPIDMode == 18 ) {
            fPID[18] = atof( KeyPadText.c_str() );
        }        
        else if( nPIDMode == 19 ) {
            fPID[19] = atof( KeyPadText.c_str() );
        }        
    }
    YellowSetUIValues();
}

void DecBWSUI::YellowGetKeyPadString() {
    if( nYelloMode == 0 ) {
        if( nLeverSpeedMode == 0 ) {
            KeyPadText = pInfoLeftLeverSpeed->_text;
        }
        if( nLeverSpeedMode == 1 ) {
            KeyPadText = pInfoRightLeverSpeed->_text;
        }
    }
    if( nYelloMode == 1 ) {
        if( nFallingLimitMode == 0 ) {
            KeyPadText = pInfoLeftFallingLimit->_text;
        }
        if( nFallingLimitMode == 1 ) {
            KeyPadText = pInfoRightFallingLimit->_text;
        }
    }
    if( nYelloMode == 2 ) {
        if( nPIDMode == 0 ) {
            KeyPadText = pInfoLeftWheelP->_text;
        }
        else if( nPIDMode == 1 ) {
            KeyPadText = pInfoLeftWheelI->_text;
        }
        else if( nPIDMode == 2 ) {
            KeyPadText = pInfoLeftWheelD->_text;
        }
        else if( nPIDMode == 3 ) {
            KeyPadText = pInfoRightWheelP->_text;
        }
        else if( nPIDMode == 4 ) {
            KeyPadText = pInfoRightWheelI->_text;
        }
        else if( nPIDMode == 5 ) {
            KeyPadText = pInfoRightWheelD->_text;
        }
        else if( nPIDMode == 6 ) {
            KeyPadText = pInfoLeftBWSP->_text;
        }
        else if( nPIDMode == 7 ) {
            KeyPadText = pInfoLeftBWSI->_text;
        }
        else if( nPIDMode == 8 ) {
            KeyPadText = pInfoLeftBWSD->_text;
        }
        else if( nPIDMode == 9 ) {
            KeyPadText = pInfoRightBWSP->_text;
        }
        else if( nPIDMode == 10 ) {
            KeyPadText = pInfoRightBWSI->_text;
        }
        else if( nPIDMode == 11 ) {
            KeyPadText = pInfoRightBWSD->_text;
        }        
        else if( nPIDMode == 12 ) {
            KeyPadText = pInfoLeftRefA->_text;
        }
        else if( nPIDMode == 13 ) {
            KeyPadText = pInfoLeftRefB->_text;
        }
        else if( nPIDMode == 14 ) {
            KeyPadText = pInfoLeftRefC->_text;
        }
        else if( nPIDMode == 15 ) {
            KeyPadText = pInfoLeftRefD->_text;
        }
        else if( nPIDMode == 16 ) {
            KeyPadText = pInfoRightRefA->_text;
        }
        else if( nPIDMode == 17 ) {
            KeyPadText = pInfoRightRefB->_text;
        }        
        else if( nPIDMode == 18 ) {
            KeyPadText = pInfoRightRefC->_text;
        }        
        else if( nPIDMode == 19 ) {
            KeyPadText = pInfoRightRefD->_text;
        }        
    }

}

void DecBWSUI::homeBWS() {
    SkDebugf( "HOME BWS\n" );
    setBWSMode( 0, 12.34, 43.21, 77.11, 11.77 );
}

void DecBWSUI::setBWSMode( int m, float d1, float d2, float d3, float d4 ) {
    SkDebugf( "Set BWS Mode: %i %0.2f %0.2f %0.2f %0.2f\n", m, d1, d2, d3, d4 );
#ifdef __EMSCRIPTEN__
    char sz[1024];
    sprintf( sz, "setBWSMode( %i, %f, %f, %f, %f )", m, d1, d2, d3, d4 );
    
    emscripten_run_script(sz);
#endif
}

void DecBWSUI::setBWSLeverSpeed( float s ) {
#ifdef __EMSCRIPTEN__
    char sz[1024];
    sprintf( sz, "setBWSLeverSpeed( %0.2f )", s );
    
    emscripten_run_script(sz);
#endif 
}


void DecBWSUI::ParamClear() {
    KeyPadText = "";
    if( nYelloMode == 0 ) {
        if( nLeverSpeedMode == 0 ) {
            pInfoLeftLeverSpeed->text( "" );
        }
        if( nLeverSpeedMode == 1 ) {
            pInfoRightLeverSpeed->text( "" );
        }
    }
    if( nYelloMode == 1 ) {
        if( nFallingLimitMode == 0 ) {
            pInfoLeftFallingLimit->text( "" );
        }
        if( nFallingLimitMode == 1 ) {
            pInfoRightFallingLimit->text( "" );
        }
    }
    if( nYelloMode == 2 ) {
        if( nPIDMode == 0 ) {
            pInfoLeftWheelP->text( "" );
        }
        else if( nPIDMode == 1 ) {
            pInfoLeftWheelI->text( "" );
        }
        else if( nPIDMode == 2 ) {
            pInfoLeftWheelD->text( "" );
        }
        else if( nPIDMode == 3 ) {
            pInfoRightWheelP->text( "" );
        }
        else if( nPIDMode == 4 ) {
            pInfoRightWheelI->text( "" );
        }
        else if( nPIDMode == 5 ) {
            pInfoRightWheelD->text( "" );
        }
        else if( nPIDMode == 6 ) {
            pInfoLeftBWSP->text( "" );
        }
        else if( nPIDMode == 7 ) {
            pInfoLeftBWSI->text( "" );
        }
        else if( nPIDMode == 8 ) {
            pInfoLeftBWSD->text( "" );
        }
        else if( nPIDMode == 9 ) {
            pInfoRightBWSP->text( "" );
        }
        else if( nPIDMode == 10 ) {
            pInfoRightBWSI->text( "" );
        }
        else if( nPIDMode == 11 ) {
            pInfoRightBWSD->text( "" );
        }        
        else if( nPIDMode == 12 ) {
            pInfoLeftRefA->text( "" );
        }
        else if( nPIDMode == 13 ) {
            pInfoLeftRefB->text( "" );
        }
        else if( nPIDMode == 14 ) {
            pInfoLeftRefC->text( "" );
        }
        else if( nPIDMode == 15 ) {
            pInfoLeftRefD->text( "" );
        }
        else if( nPIDMode == 16 ) {
            pInfoRightRefA->text( "" );
        }
        else if( nPIDMode == 17 ) {
            pInfoRightRefB->text( "" );
        }
        else if( nPIDMode == 18 ) {
            pInfoRightRefC->text( "" );
        }        
        else if( nPIDMode == 19 ) {
            pInfoRightRefD->text( "" );
        }        
    }
}

void DecBWSUI::setTrainingMode( int m ) {
    nTrainingMode = m;
    char sz[32];

    if( m == 0 ) {
        pTrainingModeStraight->borderWidth( 3 );
        pTrainingModeFollow->borderWidth( 1 );
        pTrainingModeManual->borderWidth( 1 );
        sprintf( sz, "%0.2f m/s", fMaxSpeed );
        pTrainingMaxSpeedBox->text( sz );
        _pTextModeName->text( "Straight");
    }
    else if( m == 1 ) {
        pTrainingModeStraight->borderWidth( 1 );
        pTrainingModeFollow->borderWidth( 3 );
        pTrainingModeManual->borderWidth( 1 );
        sprintf( sz, "%0.2f m/s", fMaxSpeed );
        pTrainingMaxSpeedBox->text( sz );
        _pTextModeName->text( "Following");
    }
    else if( m == 2 ) {
        pTrainingModeStraight->borderWidth( 1 );
        pTrainingModeFollow->borderWidth( 1 );
        pTrainingModeManual->borderWidth( 3 );
        sprintf( sz, "%0.2f m/s", fMaxSpeed );
        pTrainingMaxSpeedBox->text( sz );   
        _pTextModeName->text( "Manual");
    }
}

void DecBWSUI::update() {
    char sz[32];
    updateCount++;
    if( updateCount == 30 ) {
        //setBWSLeverSpeed( fLeverSpeed );
        homeBWS();
    }

    if( bR2 ) 
    {
        if( nWorkingMode == 0 ) {
            if( nRedMode == 0 ) 
            {
                pArrowLeverUp->active( false );
                pArrowLeverDown->active( false );
                if( jY > 0.5 ) {
                    pArrowLeverDown->active( true );
                    //fLeverSpeed -= 0.01;
                    setBWSMode(2, -1, 0, 0, 0 );
                }
                else if( jY < -0.5 ) {
                    pArrowLeverUp->active( true );
                    //fLeverSpeed += 0.01;
                    setBWSMode(2, 1, 0, 0, 0 );
                }
                //sprintf( sz, "%0.2f m/s", fLeverSpeed );
                pInfoLeverSpeed->text( sz ); 
                pInfoLeverSpeed->invalidate();
                pInfoLeverSpeed->dirty();
            }
            if( nRedMode == 1 ) {
                if( bDUp != bLastDUp ) {
                    if( bDUp ) {
                        pArrowWeightSupportUp->active( true );
                        if( bLeftWeight )
                            fLeftWeight += 0.5;
                        if( bRightWeight )
                            fRightWeight += 0.5;
                        sprintf( sz, "L %0.1f kg", fLeftWeight );
                        pLeftWeightSupportText->text( sz );
                        pLeftWeightSupportText2->text( sz );

                        sprintf( sz, "R %0.1f kg", fRightWeight );
                        pRightWeightSupportText->text( sz );                        
                        pRightWeightSupportText2->text( sz );                        
                    }
                    else {
                        pArrowWeightSupportUp->active( false );
                    }
                    bLastDUp = bDUp;
                }
                if( bDDown != bLastDDown ) {
                    if( bDDown ) {
                        pArrowWeightSupportDown->active( true );
                        if( bLeftWeight )
                            fLeftWeight -= 0.5;
                        if( bRightWeight )
                            fRightWeight -= 0.5;
                        sprintf( sz, "L %0.1f kg", fLeftWeight );
                        pLeftWeightSupportText->text( sz );
                        sprintf( sz, "R %0.1f kg", fRightWeight );
                        pRightWeightSupportText->text( sz );                        
                    }
                    else {
                        pArrowWeightSupportDown->active( false );
                    }
                    bLastDDown = bDDown;
                }
            }
            if( nRedMode == 2 ) {
                if( bDUp != bLastDUp ) {
                    if( bDUp ) {
                        pArrowMaxSpeedUp->active( true );
                        if( nTrainingMode == 0 ) {
                            fMaxSpeed += 0.01;
                            sprintf( sz, "%0.2f m/s", fMaxSpeed );
                            pTrainingMaxSpeedBox->text( sz );
                        }
                        else if( nTrainingMode == 1 ) {
                            fMaxSpeed += 0.01;
                            sprintf( sz, "%0.2f m/s", fMaxSpeed );
                            pTrainingMaxSpeedBox->text( sz );
                        }
                        else if( nTrainingMode == 2 ) {
                            fMaxSpeed += 0.01;
                            setBWSMode( 3, fMaxSpeed, 0, 0, 0 );
                            sprintf( sz, "%0.2f m/s", fMaxSpeed );
                            pTrainingMaxSpeedBox->text( sz );                        
                        }                    
                    }
                    else {
                        pArrowMaxSpeedUp->active( false );
                    }
                    bLastDUp = bDUp;
                }
                if( bDDown != bLastDDown ) {
                    if( bDDown ) {
                        pArrowMaxSpeedDown->active( true );
                        if( nTrainingMode == 0 ) {
                            fMaxSpeed -= 0.01;
                            sprintf( sz, "%0.2f m/s", fMaxSpeed );
                            pTrainingMaxSpeedBox->text( sz );
                        }
                        else if( nTrainingMode == 1 ) {
                            fMaxSpeed -= 0.01;
                            sprintf( sz, "%0.2f m/s", fMaxSpeed );
                            pTrainingMaxSpeedBox->text( sz );
                        }
                        else if( nTrainingMode == 2 ) {
                            fMaxSpeed -= 0.01;
                            setBWSMode( 3, fMaxSpeed, 0, 0, 0 );
                            sprintf( sz, "%0.2f m/s", fMaxSpeed );
                            pTrainingMaxSpeedBox->text( sz );                        
                        }                     

                    }
                    else {
                        pArrowMaxSpeedDown->active( false );
                    }
                    bLastDDown = bDDown;
                }
            }
        }
        if( nWorkingMode == 1 ) {
            if( nGreenMode == 0 ) 
            {
                // pArrowRightPosUp
                // nPosMode = 1;
                
                if( bDUp != bLastDUp ) {
                    if( bDUp ) {
                        if( nPosMode == 0 ) {
                            pArrowLeftPosUp->active( true );
                            fLeftPos += 5;
                            sprintf( sz, "%0.1f cm", fLeftPos );
                            pInfoLeftPos->text( sz );
                        }
                        else if( nPosMode == 1 ) {
                            pArrowRightPosUp->active( true );
                            fRightPos += 5;
                            sprintf( sz, "%0.1f cm", fRightPos );
                            pInfoRightPos->text( sz );  
                        }
                    }
                    else {
                        pArrowLeftPosUp->active( false );
                        pArrowRightPosUp->active( false );
                    }
                    bLastDUp = bDUp;
                }
                if( bDDown != bLastDDown ) {
                    if( bDDown ) {
                        if( nPosMode == 0 ) {
                            pArrowLeftPosDown->active( true );
                            fLeftPos -= 5;
                            sprintf( sz, "%0.1f cm", fLeftPos );
                            pInfoLeftPos->text( sz );                            
                        }
                        else if( nPosMode == 1 ) {
                            pArrowRightPosDown->active( true );                    
                            fRightPos -= 5;
                            sprintf( sz, "%0.1f cm", fRightPos );
                            pInfoRightPos->text( sz );                              
                        }
                    }
                    else {
                        pArrowLeftPosUp->active( false );
                        pArrowRightPosUp->active( false );
                    }
                    bLastDDown = bDDown;
                }
            }            
        }
    } 
}