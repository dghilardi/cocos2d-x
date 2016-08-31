/****************************************************************************
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#include "CCApplication-android.h"
#include "CCGLViewImpl-android.h"
#include "base/CCDirector.h"
#include "base/CCEventCustom.h"
#include "base/CCEventType.h"
#include "base/CCEventDispatcher.h"
#include "renderer/CCGLProgramCache.h"
#include "renderer/CCTextureCache.h"
#include "renderer/ccGLStateCache.h"
#include "2d/CCDrawingPrimitives.h"
#include "platform/android/jni/JniHelper.h"
#include "network/CCDownloader-android.h"
#include "AndroidMain.h"
#include <android/log.h>
#include <jni.h>
#include <gpg/android_initialization.h>
#include <gpg/android_support.h>

#define  LOG_TAG    "main"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

void cocos_android_app_init(JNIEnv* env) __attribute__((weak));

using namespace cocos2d;

extern "C"
{

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv* env;

    JniHelper::setJavaVM(vm);

    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
       return -1;
    }

    AndroidMain::OnLoad(vm);
    cocos_android_app_init(JniHelper::getEnv());

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxActivity_nativeOnActivityCreated(JNIEnv* env, jobject thiz, jobject activity, jobject savedInstanceState) {
      AndroidMain::NativeOnActivityCreated(env, activity, savedInstanceState);
}

JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxActivity_nativeOnActivityResult
    (JNIEnv* env, jobject thiz, jobject activity, jint request_code, jint result_code, jobject result) {

    // Forward activity result to GPG
    gpg::AndroidSupport::OnActivityResult(env, activity, request_code, result_code, result);

}

JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxActivity_signIn
    (JNIEnv* env, jobject thiz) {

    StateManager::BeginUserInitiatedSignIn();
}

JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxActivity_signOut
    (JNIEnv* env, jobject thiz) {

    StateManager::SignOut();
}

JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxActivity_showAchievements
  (JNIEnv *, jobject) {

    StateManager::ShowAchievements();
 }

JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxActivity_showLeaderboards
  (JNIEnv *, jobject) {

    StateManager::GetGameServices()->Leaderboards().ShowAllUI();
}

JNIEXPORT void JNICALL Java_org_cocos2dx_lib_Cocos2dxActivity_setAuthResultCallback
  (JNIEnv * env, jobject thiz, jstring callback) {

    AndroidMain::SetAuthResultCallback(env, thiz, callback);
}

JNIEXPORT void Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeInit(JNIEnv*  env, jobject thiz, jint w, jint h)
{
    auto director = cocos2d::Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview)
    {
        glview = cocos2d::GLViewImpl::create("Android app");
        glview->setFrameSize(w, h);
        director->setOpenGLView(glview);

        cocos2d::Application::getInstance()->run();
    }
    else
    {
        cocos2d::GL::invalidateStateCache();
        cocos2d::GLProgramCache::getInstance()->reloadDefaultGLPrograms();
        cocos2d::DrawPrimitives::init();
        cocos2d::VolatileTextureMgr::reloadAllTextures();

        cocos2d::EventCustom recreatedEvent(EVENT_RENDERER_RECREATED);
        director->getEventDispatcher()->dispatchEvent(&recreatedEvent);
        director->setGLDefaultValues();
    }
    cocos2d::network::_preloadJavaDownloaderClass();
}

JNIEXPORT jintArray Java_org_cocos2dx_lib_Cocos2dxActivity_getGLContextAttrs(JNIEnv*  env, jobject thiz)
{
    cocos2d::Application::getInstance()->initGLContextAttrs();
    GLContextAttrs _glContextAttrs = GLView::getGLContextAttrs();

    int tmp[6] = {_glContextAttrs.redBits, _glContextAttrs.greenBits, _glContextAttrs.blueBits,
                           _glContextAttrs.alphaBits, _glContextAttrs.depthBits, _glContextAttrs.stencilBits};


    jintArray glContextAttrsJava = env->NewIntArray(6);
        env->SetIntArrayRegion(glContextAttrsJava, 0, 6, tmp);

    return glContextAttrsJava;
}

JNIEXPORT void Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeOnSurfaceChanged(JNIEnv*  env, jobject thiz, jint w, jint h)
{
    cocos2d::Application::getInstance()->applicationScreenSizeChanged(w, h);
}

}

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
