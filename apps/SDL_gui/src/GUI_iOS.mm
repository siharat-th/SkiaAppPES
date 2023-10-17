//
//  GUI_iOS.m
//  iOS
//
//  Created by Jimmy on 1/4/2559 BE.
//  Copyright © 2559 Jimmy Software Co.,Ltd. All rights reserved.
//

#import "GUI_iOS.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Jimmy
char __iOS_DOCUMENTS_FOLDER[512];

// Jimmy
NSString *dir = [NSString stringWithFormat:@"%@/Documents", NSHomeDirectory()];
//NSLog( @"Dir: %@", dir );

extern "C" void GUI_iOS_Init() {
    strcpy( __iOS_DOCUMENTS_FOLDER, [dir UTF8String] );
}

@implementation GUI_iOS
    @end

#ifdef __IPHONEOS__

//Loading for iOS
extern "C" void ShowLoading(bool show) {
        //NSLog(@"ShowLoader:Called");
        UIWindow *window = [[[UIApplication sharedApplication] delegate] window];
        SDL_uikitviewcontroller *vc = (SDL_uikitviewcontroller*)window.rootViewController;
        if(show && vc.actv == nil){
            NSLog(@"ShowLoader:true");
            vc.actv = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:(UIActivityIndicatorViewStyleGray)];
            vc.actv.center = window.rootViewController.view.center;
            vc.actv.layer.borderColor = vc.actv.layer.backgroundColor = [UIColor whiteColor].CGColor;
            vc.actv.layer.cornerRadius = 4.0f;
            vc.actv.layer.borderWidth = 1.0f;
            [vc.actv startAnimating];
            [window.rootViewController.view addSubview:vc.actv];
        }
        else if(!show && vc.actv != nil){
            NSLog(@"ShowLoader:false");
            [vc.actv removeFromSuperview];
            vc.actv = nil;
        }
}

@interface UIImage (fixOrientation)
- (UIImage *)fixOrientation;
- (UIImage *)resizeImage:(UIImage *)image withMaxDimension:(CGFloat)maxDimension;
@end


//---------
//ART - define Custom ImagePickerController with landscape only
@interface NonRotatingUIImagePickerController : UIImagePickerController
@end
@implementation NonRotatingUIImagePickerController
    // Disable Landscape mode.
- (UIInterfaceOrientationMask)supportedInterfaceOrientations{
    return UIInterfaceOrientationMaskLandscape;
}
@end
//---------
//ART - define Camera & Gallery function
@implementation imagePickerDelegateHolder
- (void)imagePickerController:(UIImagePickerController *)picker
didFinishPickingMediaWithInfo:(NSDictionary<UIImagePickerControllerInfoKey, id> *)info{
    NSLog(@"imagePickerController delegate called");
    ShowLoading(true);
    [picker dismissViewControllerAnimated:true completion:^{
        //save image to file
        UIImage* image;
        if([[info valueForKey:@"UIImagePickerControllerMediaType"] isEqualToString:@"public.image"])
        {
            image = [info valueForKey:@"UIImagePickerControllerOriginalImage"];
            NSString *stringPath = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES)objectAtIndex:0]stringByAppendingPathComponent:@"ImportImageiOS"];
            NSError *error = nil;
            if (![[NSFileManager defaultManager] fileExistsAtPath:stringPath])
            [[NSFileManager defaultManager] createDirectoryAtPath:stringPath withIntermediateDirectories:NO attributes:nil error:&error];
            NSString *fileName = [stringPath stringByAppendingFormat:@"/%@.png",[NSUUID UUID].UUIDString];
            NSLog(@"Size %f || %f",image.size.width,image.size.height);
            if(image.size.width > 2000 || image.size.height > 2000 )
            {
                image = [image resizeImage:image withMaxDimension:1200];
                NSLog(@"Resize to %f || %f",image.size.width,image.size.height);
            }
            
            image = [image fixOrientation];
            NSData *data = UIImagePNGRepresentation(image);
            [data writeToFile:fileName atomically:YES];
            importPNGiOS([fileName cStringUsingEncoding:NSUTF8StringEncoding]);
            //delete image&Folder
            [[NSFileManager defaultManager] removeItemAtPath:fileName error:nil];
            [[NSFileManager defaultManager] removeItemAtPath:stringPath error:nil];
            ShowLoading(false);
        }
    }];
}
- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
    NSLog(@"imagePickerController delegate cancel");
    [picker dismissViewControllerAnimated:true completion:nil];
}
@end
//---------



extern "C" void ShareFile( const char *filePath ) {
    printf("Share file: %s\n", filePath );
    try {
        NSURL *fileURL = [NSURL fileURLWithPath:[NSString stringWithUTF8String:filePath]];
        
        NSArray *items = @[fileURL];
        
        // build an activity view controller
        UIActivityViewController *controller = [[UIActivityViewController alloc]initWithActivityItems:items applicationActivities:nil];
        /*
        [self presentViewController:controller animated:YES completion:^{
            // executes after the user selects something
        }];
        */
        UIWindow *window = [[[UIApplication sharedApplication] delegate] window];
        UIViewController *vc = window.rootViewController;
        //UIView *topView = window.rootViewController.view;
        
        //if iPhone
        if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone) {
            [vc presentViewController:controller animated:YES completion:nil];
        }
        //if iPad
        else {
            // Change Rect to position Popover
            UIPopoverController *popup = [[UIPopoverController alloc] initWithContentViewController:controller];
            [popup presentPopoverFromRect:CGRectMake(vc.view.frame.size.width/2, vc.view.frame.size.height/4, 0, 0)inView:vc.view permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
        }

        /*
        // for iPad: make the presentation a Popover
        controller.modalPresentationStyle = UIModalPresentationPopover;
        [vc presentViewController:controller animated:YES completion:nil];
        
        //UIPopoverPresentationController *popController = [controller popoverPresentationController];
        //popController.permittedArrowDirections = UIPopoverArrowDirectionAny;
        //popController.barButtonItem = vc.navigationItem.leftBarButtonItem;
        */
        
        /*
        NSArray *objectsToShare = [fileURL]
        let activityVC = UIActivityViewController(activityItems: objectsToShare, applicationActivities: nil)
        
        self.presentViewController(activityVC, animated: true, completion: nil)
         */
        
    } catch(NSException *ex) {
        printf("cannot write file");
        // failed to write file – bad permissions, bad filename, missing permissions, or more likely it can't be converted to the encoding
    }
}

//ART - Open Camera & gallery
#define SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(v)  ([[[UIDevice currentDevice] systemVersion] compare:v options:NSNumericSearch] != NSOrderedAscending)
extern "C" void openiOSCameraOrGallery(bool camera) {

    UIWindow *window = [[[UIApplication sharedApplication] delegate] window];
    SDL_uikitviewcontroller *vc = (SDL_uikitviewcontroller*)window.rootViewController;
    vc.imagePicker = [[NonRotatingUIImagePickerController alloc] init];
    vc.imagePickerDelegateHolder = [[imagePickerDelegateHolder alloc] init];
    vc.imagePicker.delegate = vc.imagePickerDelegateHolder;
    if(camera){
       vc.imagePicker.sourceType = UIImagePickerControllerSourceTypeCamera;
    }
    else{
        vc.imagePicker.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
    }
    
    if (SYSTEM_VERSION_GREATER_THAN_OR_EQUAL_TO(@"13")
        || [[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        //iOS 13+ or iOS12- iPhone
        [window.rootViewController presentViewController:vc.imagePicker animated:YES completion:nil];
    }else{
        //For iOS12- iPad
        vc.imagePicker.modalPresentationStyle = UIModalPresentationPopover;
        vc.imagePicker.popoverPresentationController.sourceView = vc.imagePicker.view;
        vc.imagePicker.popoverPresentationController.sourceRect = CGRectMake(vc.imagePicker.view.frame.size.width/2, vc.imagePicker.view.frame.size.height/4, 0, 0);
        [window.rootViewController  presentViewController:vc.imagePicker animated: YES completion: nil];
    }
}

@implementation UIImage (fixOrientation)
- (UIImage *)resizeImage:(UIImage *)image withMaxDimension:(CGFloat)maxDimension
    {
        if (fmax(image.size.width, image.size.height) <= maxDimension) {
            return image;
        }
        
        CGFloat aspect = image.size.width / image.size.height;
        CGSize newSize;
        
        if (image.size.width > image.size.height) {
            newSize = CGSizeMake(maxDimension, maxDimension / aspect);
        } else {
            newSize = CGSizeMake(maxDimension * aspect, maxDimension);
        }
        
        UIGraphicsBeginImageContextWithOptions(newSize, NO, 1.0);
        CGRect newImageRect = CGRectMake(0.0, 0.0, newSize.width, newSize.height);
        [image drawInRect:newImageRect];
        UIImage *newImage = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
        
        return newImage;
    }
    
- (UIImage *)fixOrientation {
    if (self.imageOrientation == UIImageOrientationUp) return self;
    CGAffineTransform transform = CGAffineTransformIdentity;
    switch (self.imageOrientation) {
        case UIImageOrientationDown:
        case UIImageOrientationDownMirrored:
        transform = CGAffineTransformTranslate(transform, self.size.width, self.size.height);
        transform = CGAffineTransformRotate(transform, M_PI);
        break;
        
        case UIImageOrientationLeft:
        case UIImageOrientationLeftMirrored:
        transform = CGAffineTransformTranslate(transform, self.size.width, 0);
        transform = CGAffineTransformRotate(transform, M_PI_2);
        break;
        
        case UIImageOrientationRight:
        case UIImageOrientationRightMirrored:
        transform = CGAffineTransformTranslate(transform, 0, self.size.height);
        transform = CGAffineTransformRotate(transform, -M_PI_2);
        break;
        case UIImageOrientationUp:
        case UIImageOrientationUpMirrored:
        break;
    }
    
    switch (self.imageOrientation) {
        case UIImageOrientationUpMirrored:
        case UIImageOrientationDownMirrored:
        transform = CGAffineTransformTranslate(transform, self.size.width, 0);
        transform = CGAffineTransformScale(transform, -1, 1);
        break;
        
        case UIImageOrientationLeftMirrored:
        case UIImageOrientationRightMirrored:
        transform = CGAffineTransformTranslate(transform, self.size.height, 0);
        transform = CGAffineTransformScale(transform, -1, 1);
        break;
        case UIImageOrientationUp:
        case UIImageOrientationDown:
        case UIImageOrientationLeft:
        case UIImageOrientationRight:
        break;
    }
    CGContextRef ctx = CGBitmapContextCreate(NULL, self.size.width, self.size.height,
                                             CGImageGetBitsPerComponent(self.CGImage), 0,
                                             CGImageGetColorSpace(self.CGImage),
                                             CGImageGetBitmapInfo(self.CGImage));
    CGContextConcatCTM(ctx, transform);
    switch (self.imageOrientation) {
        case UIImageOrientationLeft:
        case UIImageOrientationLeftMirrored:
        case UIImageOrientationRight:
        case UIImageOrientationRightMirrored:
        // Grr...
        CGContextDrawImage(ctx, CGRectMake(0,0,self.size.height,self.size.width), self.CGImage);
        break;
        
        default:
        CGContextDrawImage(ctx, CGRectMake(0,0,self.size.width,self.size.height), self.CGImage);
        break;
    }
    
    CGImageRef cgimg = CGBitmapContextCreateImage(ctx);
    UIImage *img = [UIImage imageWithCGImage:cgimg];
    CGContextRelease(ctx);
    CGImageRelease(cgimg);
    return img;
}
@end
#endif /*__IPHONEOS__*/
