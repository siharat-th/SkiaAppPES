//
//  GUI_iOS.h
//  iOS
//
//  Created by Jimmy on 1/4/2559 BE.
//  Copyright © 2559 Jimmy Software Co.,Ltd. All rights reserved.
//

#import <Foundation/Foundation.h>
#ifdef __IPHONEOS__
#include "PES_File.hpp"
#import <UIKit/UIKit.h>
#endif

@interface GUI_iOS : NSObject
@end

//--------------------
//ART - ImagePickerDelegate
#ifdef __IPHONEOS__
@interface SDL_uikitviewcontroller : UIViewController
@property (nonatomic, retain) UIImagePickerController *imagePicker;
@property (nonatomic, retain) NSObject  *imagePickerDelegateHolder;
@property (nonatomic, retain) UIActivityIndicatorView *actv;
@end

@interface imagePickerDelegateHolder : NSObject
<UINavigationControllerDelegate,UIImagePickerControllerDelegate>
@end
#endif
//--------------------
 

