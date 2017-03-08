#ifdef ANDROID
	#include <jni.h>
	#include <android_native_app_glue.h>

	#include <android/log.h>
	#include <string>

	#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, projectName, __VA_ARGS__))
	#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, projectName, __VA_ARGS__))
#endif

#ifdef _WINDOWS
	#include "windows.h"
	#include <sstream>
	#include <fstream>
#endif

#include "tbal.h"
#include "time.h"
#include "assert.h"
#include "tbarr.h"
#include "algorithm.h"

namespace tbal
{		
	static const char *projectName = "TbalProject";
	static const char *hardCmdLine = NULL;
	static bool started = false;
	static bool enabled_buttons = true;
	static bool enabled_mouse = true;

	const char* SetProjectName (const char* newName)
	{
		const char* result = projectName;
		if (newName) projectName = newName;
		return result;
	}
	
	const char* SetHardCmdLine (const char* newHardCmdLine)
	{
		const char* result = hardCmdLine;
		if (newHardCmdLine) hardCmdLine = newHardCmdLine;
		return result;
	}

	void EnableButtonDraw (bool enable)
	{
		enabled_buttons = enable;
	}
	
	void EnableMouse (bool enable)
	{
		enabled_mouse = enable;
	}

	struct ScreenButtonInfo
	{
		Code code;
		int number, px,py,sx,sy;
		bool visible;
		bool pushed;
		bool creepable;
		Color transparentColor;
		Picture	image, imagePushed;

		Code GetCode () { return this ? code : CODE_POINTER; }
	};

	std::vector<ScreenButtonInfo> screenButtons;

	void SetButtonImages(int number, Color transparentColor, const Picture& image, const Picture& imagePushed)
	{
		if (size_t(number) < screenButtons.size())
		{
			ScreenButtonInfo& sbi = screenButtons[size_t(number)];
			sbi.transparentColor = transparentColor;
			sbi.image = image;
			sbi.imagePushed = imagePushed;
		}
	}
	
	int CreateScreenButton (Code code, int px, int py, int sx, int sy, Color transparentColor, const Picture& image, const Picture& imagePushed, bool creepable)
	{
		ScreenButtonInfo newElement;
		newElement.code = code;
		newElement.px = px;
		newElement.py = py;
		newElement.sx = sx;
		newElement.sy = sy;
		newElement.visible   = false;
		newElement.pushed    = false;
		newElement.creepable = creepable;
		newElement.number = (int)screenButtons.size();
		int result = (int)screenButtons.size();
		screenButtons.push_back(newElement);
		SetButtonImages(result, transparentColor, image, imagePushed);
		return result;
	}
	
	void GetButtonPosition (int number, int &px, int &py, int &sx, int &sy)
	{
		if (size_t(number) < screenButtons.size())
		{
			px=screenButtons[number].px;
			py=screenButtons[number].py;
			sx=screenButtons[number].sx;
			sy=screenButtons[number].sy;
		} else
			px=py=sx=sy=0;
	}
		
	
	void SetButtonPosition (int number, int  px, int  py, int  sx, int  sy)
	{
		if (size_t(number) < screenButtons.size())
		{
			if (screenButtons[number].sx != sx || screenButtons[number].sy != sy)
				SetButtonImages(number, screenButtons[number].transparentColor, Picture(0,0), Picture(0,0)); 
			screenButtons[number].px=px;
			screenButtons[number].py=py;
			screenButtons[number].sx=sx;
			screenButtons[number].sy=sy;
		}
	}
	
	bool ShowButton (int number, bool show)
	{
		bool oldShow = screenButtons[number].visible;
		screenButtons[number].visible = show;
		return oldShow;
	}

	tblib::carray<bool,0x400>	pressed;
	bool lPressed = false;
	int	 interval = -1;
	int  locked   = 0;
	ScreenButtonInfo	*lastButton   = NULL;

	bool Pressed (Code code) 
	{
		return pressed[code];
	}

	bool Test (const ScreenButtonInfo& button, int x, int y)
	{
		return (button.visible && x>=button.px && y>=button.py && x<button.px+button.sx && y<button.py+button.sy); 
	}

	ScreenButtonInfo* TestButtons (int x, int y)
	{
		for (int i = 0; i<(int)screenButtons.size(); ++i)
			if (Test(screenButtons[i], x, y)) 
				return &screenButtons[i];
		return NULL;
	}

	void DrawButton (const Bitmap& buffer, int number)
	{
		if (size_t(number) < screenButtons.size())
		{
			ScreenButtonInfo &button = screenButtons[number];
			buffer
				.Copy(
				TransparentTest(button.transparentColor),	
				button.pushed ? button.imagePushed : button.image,
				button.px, button.py);
		}
	}	

	void DrawAllButtons (const Bitmap& buffer)
	{	
		for (int i=0; i < (int)screenButtons.size(); ++i)
			if (screenButtons[i].visible)
				DrawButton (buffer, i);
	}

#ifdef _WINDOWS	
	HWND window = NULL;

	void ClipPointer (bool clip)
	{
		if (clip)
		{
			RECT r;
			GetClientRect(window, &r);
			ClientToScreen(window, reinterpret_cast<LPPOINT>(&(r.left)));
			ClientToScreen(window, reinterpret_cast<LPPOINT>(&(r.right)));	
			ClipCursor(&r);
		} else
			ClipCursor(NULL);
	};
#endif
#ifdef ANDROID
	void ClipPointer (bool clip)
	{
		(void)clip;
	}
#endif

	bool PointerDown(int x, int y)
	{
		ClipPointer(true);
		ScreenButtonInfo *n = TestButtons(x,y);
		lPressed = true;

		if (n) n->pushed = true;
		lastButton = n;
		pressed [n->GetCode()] = true;
		return TbalMain(ACTION_DOWN, n->GetCode(), x, y);
	}

	bool PointerMove(int x, int y)
	{
		if (!lPressed)
			#ifdef TBAL_MOUSE_OVER
				return TbalMain(ACTION_MOVE, CODE_POINTER, x, y);
			#else
				return true;
			#endif

		ScreenButtonInfo *n = TestButtons(x,y);
		bool oldPointer = pressed [CODE_POINTER];
		if (n && n != lastButton && !n->creepable) n=NULL;        
		if (n!=lastButton)
		{
			// мы съехали с этой кнопки
			pressed[lastButton->GetCode()] = false;
			if (lastButton) 
			{
				lastButton->pushed = false;
				if (!TbalMain(ACTION_LOST, lastButton->GetCode(), x,y)) return false;
			}
			lastButton = n;
			//мы опять попали по кнопке
			if (n)
			{
				pressed[n->GetCode()] = true;
				n->pushed = true;
				if (!TbalMain(ACTION_DOWN, n->GetCode(), x, y)) return false;
			}
		} else if (oldPointer && pressed[CODE_POINTER]) // если просто двинули мышкой
			if (!TbalMain(ACTION_MOVE, n->GetCode(), x,y)) return false;
		return true;
	}	

	bool PointerUp(int x, int y)
	{
		#ifndef TBAL_MOUSE_OVER
			ClipPointer(false);
		#endif
		lPressed = false;

		if (lastButton || pressed[CODE_POINTER])
		{
			pressed[lastButton->GetCode()] = false;
			if (lastButton)
			{
				lastButton->pushed = false;
			}
			ScreenButtonInfo* lb = lastButton;
			lastButton = NULL;
			return TbalMain(ACTION_UP, lb->GetCode(), x, y);
		} else
			return true;
	}

	bool KeyDown(Code code)
	{
		pressed[code] = true;
		return TbalMain(ACTION_DOWN, code, -1, -1);
	}	

	bool KeyUp(Code code)
	{
		pressed[code] = false;
		return TbalMain(ACTION_UP, code, -1, -1);
	}
	
	bool SendTimerMessage ()
	{
		static int curTime = MSec();
		int prevTime = curTime;
		curTime = MSec();
		return TbalMain(ACTION_MOVE, CODE_TIMER, curTime - prevTime, -1);
	}

#ifdef ANDROID
/* Тут всякое андроидное говно, я в нём ничего не понимаю, просто
 * переписал дословно откуда-то
 */
	#ifdef pf16bits
		const int coolFormat = WINDOW_FORMAT_RGB_565;
	#else
		const int coolFormat = WINDOW_FORMAT_RGBX_8888;
	#endif

	android_app* appState;
	JNIEnv* env;
	bool running=true, starting = true, active = false;
	
	android_app * GetState () { return appState; }

	void EngineHandleCmd(android_app* app, int32_t cmd)
	{
		switch (cmd) {
		case APP_CMD_INIT_WINDOW:
			LOGI ("APP_CMD_INIT_WINDOW");

			if (starting && app->window != NULL)
			{
				ANativeWindow_setBuffersGeometry(app->window, 0, 0, coolFormat);
				if (app->window != NULL)
				{
					ANativeWindow_Buffer b;
					// а вдруг так повезёт, а то какого-то хуя getWidth и getHeight частенько возвращают 1
					if (ANativeWindow_lock(app->window, &b, NULL)>=0)
					{
						int format = ANativeWindow_getFormat(app->window);
						LOGI("pixelFormat=%d, size=(%d, %d)", format, b.width, b.height);
						ANativeWindow_unlockAndPost(app->window);
						if (format != coolFormat)
						{
							LOGW("invalid pixelFormat!");
							running = false;
						}
						starting = false;
					}
				}
			}
			break;
		case APP_CMD_GAINED_FOCUS:
			LOGI ("APP_CMD_GAINED_FOCUS");
			//if (!starting && !locked)
			//	Buffer b;
			active = true;
			break;
		case APP_CMD_LOST_FOCUS:
			LOGI ("APP_CMD_LOST_FOCUS");
			active  = false;
			if (!TbalMain(ACTION_MOVE, CODE_LOSTFOCUS, -1, -1))
				running = false;
			break;
		case APP_CMD_TERM_WINDOW:
			LOGI ("APP_CMD_TERM_WINDOW");
			active  = false;
			if (!TbalMain(ACTION_MOVE, CODE_LOSTFOCUS, -1, -1))
				running = false;
			break;
		case APP_CMD_CONFIG_CHANGED:
			LOGI ("APP_CMD_CONFIG_CHANGED");
			break;
		default : LOGI ("Didnt process this code %x", cmd); break;
		}
	}

	int32_t EngineHandleInput(android_app* app, AInputEvent* event)
	{
		if (!active) return 0;
		if (enabled_mouse && AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
		{
			switch(AMotionEvent_getAction(event))
			{
			case AMOTION_EVENT_ACTION_DOWN : running = running && PointerDown(AMotionEvent_getX(event,0), AMotionEvent_getY(event,0)); break;
			case AMOTION_EVENT_ACTION_MOVE : running = running && PointerMove(AMotionEvent_getX(event,0), AMotionEvent_getY(event,0)); break;
			case AMOTION_EVENT_ACTION_UP   : running = running && PointerUp  (AMotionEvent_getX(event,0), AMotionEvent_getY(event,0)); break;
			}
			return 1;
		} else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
		{
			LOGI("AINPUT_EVENT_TYPE_KEY code=%d", AKeyEvent_getKeyCode(event));
			switch (AKeyEvent_getAction(event))
			{
			case AKEY_EVENT_ACTION_DOWN : running = running && KeyDown(Code(AKeyEvent_getKeyCode(event))); break;
			case AKEY_EVENT_ACTION_UP   : running = running && KeyUp  (Code(AKeyEvent_getKeyCode(event))); break;
			}
			return 1;
		}
		return 0;
	}		

	void SetTimer (int interval)
	{
		tbal::interval = interval;
	}

	Picture singleBuffer;

	bool ProcessOne (android_app *state)
	{
		int ident;
		int events;
		android_poll_source* source;
		bool ostarting = starting, orunning = running;
		
		if ((ident=ALooper_pollAll(interval, NULL, &events, (void**)&source)) >= 0)
		{
			if (source != NULL)
				source->process(state, source);
			if (state->destroyRequested != 0)
			{
				LOGI ("state->destroyRequested != 0");
				running = false;
				return false;
			}
			if (starting != ostarting || running != orunning)
			{
				LOGI ("starting != ostarting || running != orunning");
				return false;
			}
		}
		if (!starting && active && running && interval>=0 && !SendTimerMessage())
		{
			LOGI ("!starting && active && running && interval>=0 && !SendTimerMessage()");
			return false;
		}
		return true;
	}

	void InitApplication (android_app* state)
	{
		app_dummy();
		LOGI("tbal::InitApplication");
		tbal::projectName = projectName;
		appState = state;
		state->userData     = NULL;
		state->onAppCmd     = EngineHandleCmd;
		state->onInputEvent = EngineHandleInput;
		//state->activity->callbacks->onContentRectChanged = EngineHandleResized;
		state->activity->vm->AttachCurrentThread(&env, NULL);
		LOGI("tbal::StartingApplication");
		while (starting && ProcessOne(state));
	}

	void RunApplication (android_app* state)
	{
		LOGI("tbal::RunApplication");
		while (running && ProcessOne(state));
		TbalMain(ACTION_DOWN, CODE_EXIT, -1, -1);
		LOGI("end of tbal::RunApplication");
		state->activity->vm->DetachCurrentThread();
		ANativeActivity_finish(state->activity);
		LOGI("finished activity");
		exit(0);
	}

	void LogI(const char* c)
	{
		LOGI("%s", c);
	}
	
	void LogW(const char* c)
	{
		LOGI("%s", c);
	}

	const char *GetExternalFilesDir ()
	{
		assert (started);
		static std::string s="";
		if (s.length()==0)
		{
			LOGI("Try get external files dir");
			jclass c; jmethodID m; jobject o;

			c = env->FindClass        ("android/app/NativeActivity");
			m = env->GetMethodID      (c, "getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;");
			o = env->CallObjectMethod	(appState->activity->clazz, m, NULL);

			c = env->GetObjectClass		(o);
			m = env->GetMethodID    	(c, "getAbsolutePath", "()Ljava/lang/String;");
			o = env->CallObjectMethod	(o, m);
			jstring jo = (jstring)o;

			const char *path = env->GetStringUTFChars(jo, NULL);
			s=path;
			env->ReleaseStringUTFChars(jo, path);
			s+='/';
			LOGI("Path for program's data files is %s", s.c_str());
		}
		return s.c_str();
	}
	
	void RunUrl (const char* url)
	{
		jclass c,cu; 
		jmethodID m; 
		jobject o, ou,oi, oa, u; 
		jfieldID f; 
		jstring s; 

		LOGI("Run url %s", url);

		c  = env->FindClass("android/content/Intent");
		m  = env->GetMethodID		(c, "<init>", "(Ljava/lang/String;)V");
		f  = env->GetStaticFieldID ( c, "ACTION_VIEW", "Ljava/lang/String;");
		s = (jstring)(env->GetStaticObjectField(c, f));
		o = env->NewObject(c, m, s);

		cu = env->FindClass("android/net/Uri");
		m = env->GetStaticMethodID(cu, "parse", "(Ljava/lang/String;)Landroid/net/Uri;");

		static std::string adr;
		adr = url;

		s = env->NewStringUTF(adr.c_str());
		ou = env->CallStaticObjectMethod(cu, m, s);
		m = env->GetMethodID(c, "setData", "(Landroid/net/Uri;)Landroid/content/Intent;");
		oi = env->CallObjectMethod(o, m, ou);

		cu = env->FindClass        ("android/app/NativeActivity");
		m = env->GetMethodID(cu, "startActivity", "(Landroid/content/Intent;)V");
		env->CallVoidMethod(appState->activity->clazz, m, o);
	}

	float xdpi=-1.0, ydpi=-1.0;

	void GetDPI ()
	{
		jclass c; jmethodID m; jobject o, dm; jfieldID fxdpi, fydpi, fsizex, fsizey;

		c = env->FindClass			("android/util/DisplayMetrics");
		m = env->GetMethodID		(c, "<init>", "()V");
		dm = env->NewObject			(c, m);

		fxdpi  = env->GetFieldID		(c, "xdpi", "F");
		fydpi  = env->GetFieldID		(c, "ydpi", "F");

		c = env->FindClass			("android/app/NativeActivity");
		m = env->GetMethodID		(c, "getWindowManager", "()Landroid/view/WindowManager;");
		o = env->CallObjectMethod	(appState->activity->clazz, m);

		c = env->GetObjectClass		(o);
		m = env->GetMethodID		(c, "getDefaultDisplay", "()Landroid/view/Display;");
		o = env->CallObjectMethod	(o, m);

		c = env->GetObjectClass		(o);
		m = env->GetMethodID		(c, "getMetrics", "(Landroid/util/DisplayMetrics;)V");
		o = env->CallObjectMethod	(o, m, dm);
		// getRealMetrics поддерживается только с 4.2

		xdpi = env->GetFloatField	(dm, fxdpi);
		ydpi = env->GetFloatField	(dm, fydpi);
		LOGI("dpi is (%f, %f)", xdpi, ydpi);
	}

	float GetXDPI ()
	{
		if (xdpi <= 0.0) GetDPI ();
		return xdpi;
	}

	float GetYDPI ()
	{
		if (ydpi <= 0.0) GetDPI ();
		return ydpi;
	}

	void LockBitmap (Bitmap &b)
	{
		ANativeWindow_Buffer wb;
		if (ANativeWindow_lock(appState->window, &wb, NULL)>=0)
		{
			tblib::recreate(b, wb.width, wb.height, wb.stride, reinterpret_cast<Color*>(wb.bits));
		} else
		{
			LOGW("ANativeWindow_lock failed!!!");
			tblib::recreate(b, -1, -1, 0, reinterpret_cast<Color*>(NULL));
		}
	}

    
	Buffer::Buffer()
	{		
		
		if (locked) 
			LOGW ("already locked!!!");
		LockBitmap(*this);
		++locked;
	}

	Buffer::~Buffer()
	{		
		if (!locked) 
			LOGW ("already unlocked!!!");
		if (sizeX()<0 || sizeY()<0)
		{
			LOGW("empty buffer!");
		} else
		{
			if (enabled_buttons)
				DrawAllButtons(*this);
			
			if (sizeX()>10 && sizeY()>10) 
			{
				int fy = sizeY()-9;
				(*this)[fy+1][3]=COLOR_WHITE;
				(*this)[fy+1][4]=COLOR_WHITE;
				(*this)[fy+1][7]=COLOR_WHITE;
				(*this)[fy+2][3]=COLOR_WHITE;
				(*this)[fy+2][4]=COLOR_WHITE;
				(*this)[fy+2][7]=COLOR_WHITE;
				(*this)[fy+2][8]=COLOR_WHITE;
				(*this)[fy+3][8]=COLOR_WHITE;
				(*this)[fy+3][9]=COLOR_WHITE;
				(*this)[fy+5][5]=COLOR_WHITE;
				(*this)[fy+6][2]=COLOR_WHITE;
				(*this)[fy+6][3]=COLOR_WHITE;
				(*this)[fy+6][4]=COLOR_WHITE;
				(*this)[fy+6][6]=COLOR_WHITE;
				(*this)[fy+6][7]=COLOR_WHITE;
				(*this)[fy+6][8]=COLOR_WHITE;
				(*this)[fy+7][1]=COLOR_WHITE;
				(*this)[fy+7][9]=COLOR_WHITE;
			}
			ANativeWindow_unlockAndPost(appState->window);
			--locked;
		}
	}

	Asset::Asset()
	{
		asset = NULL;
	}
	
	void Asset::Open(tblib::stringref filename)
	{
		AAssetManager* assetManager = appState->activity->assetManager;
		asset = AAssetManager_open(assetManager, std::string(filename).c_str(), AASSET_MODE_UNKNOWN);
	}

	tblib::stringref Asset::GetBuffer()
	{
    const char* begin = (const char*)AAsset_getBuffer(asset);
		size_t length = AAsset_getLength(asset);

		return tblib::stringref(begin, 0, length);
	}

	void Asset::GetDescriptor(FileDescriptor& fd)
	{
		fd.descr = AAsset_openFileDescriptor(asset, &fd.start, &fd.length);
	}

	Asset::~Asset() 
	{		
		AAsset_close(asset);
	}
	
	int MSec() 
	{
		struct timezone tzz;
		struct timeval  tv;
		tzz.tz_minuteswest = 0;
		tzz.tz_dsttime     = 0;
		gettimeofday(&tv, &tzz );
		return tv.tv_sec*1000 + tv.tv_usec/1000;
	}; 
#endif

#ifdef _WINDOWS
/***************************************************************************************************
 *
 *  Вот отсюда и до конца файла идёт всякое чисто виндовое говно
 *
 ***************************************************************************************************/
	
// немного виндового говна	
	

		#ifdef TBAL_VERTICAL
			#define TBAL_FIXED_SIZE
		#endif
		#ifdef TBAL_HORIZONTAL
			#define TBAL_FIXED_SIZE
		#endif

	struct WinBitmap
	{
		tblib::array <Bitmap,2> buffers;
		tblib::array <HBITMAP,2> handle;
		tblib::array <HDC,2> dc;
		int currentBuffer;

		WinBitmap () 
		{
			for (int i=0; i<buffers.capacity; ++i)
			{
				buffers.push_back(Bitmap(0,0,0,NULL));
				handle.push_back(NULL);
				dc.push_back(NULL);
			}
		}

		~WinBitmap () 
		{            
			for (int i=0; i<2; ++i) if (handle[i]) 
			{
				DeleteDC(dc[i]);
				DeleteObject(handle[i]);
				handle[i] = 0;
			}
		}

		void Init (int sizeX, int sizeY, int bits)
		{
			tblib::recreate(*this);

			// стандартная тупая байда по заполнению полей винапишной структуры
			int pixelsPerInt = sizeof(int)/sizeof(Color);

			// отрицательное смещение строки, так как в винде видеопамять изображения перевёрнута
			int stride = ((sizeX+pixelsPerInt-1)&(~(pixelsPerInt-1)));

			BITMAPINFO bi;
			memset(&bi, 0, sizeof(bi));
			bi.bmiHeader.biSize     = sizeof(bi.bmiHeader);
			bi.bmiHeader.biWidth    = sizeX;
			bi.bmiHeader.biHeight   = -sizeY;
			bi.bmiHeader.biPlanes   = 1;
			bi.bmiHeader.biBitCount = WORD(bits);
			HDC screenDC = GetDC(NULL);
			// создаём описатели
			for (int i=0; i<2; ++i)
			{
				dc[i]     = CreateCompatibleDC(screenDC);
				Color* pixels;
				handle[i] = CreateDIBSection(dc[i], &bi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pixels), 0, 0);
				SelectObject(dc[i], handle[i]);
				// начало пикселей ставим на верхнюю строку		
				tblib::recreate(buffers[i], sizeX, sizeY, stride, pixels);
			}
			ReleaseDC(NULL, screenDC);
			currentBuffer = 0;
		}

		void SwapBuffers ()
		{
			Bitmap &b = buffers[currentBuffer];
			if (enabled_buttons)
				DrawAllButtons(b);
			if (b.sizeX()>10 && b.sizeY()>10) 
			{
				int fy = b.sizeY()-9;
				b[fy+1][3]=COLOR_WHITE;
				b[fy+1][4]=COLOR_WHITE;
				b[fy+1][7]=COLOR_WHITE;
				b[fy+2][3]=COLOR_WHITE;
				b[fy+2][4]=COLOR_WHITE;
				b[fy+2][7]=COLOR_WHITE;
				b[fy+2][8]=COLOR_WHITE;
				b[fy+3][8]=COLOR_WHITE;
				b[fy+3][9]=COLOR_WHITE;
				b[fy+5][5]=COLOR_WHITE;
				b[fy+6][2]=COLOR_WHITE;
				b[fy+6][3]=COLOR_WHITE;
				b[fy+6][4]=COLOR_WHITE;
				b[fy+6][6]=COLOR_WHITE;
				b[fy+6][7]=COLOR_WHITE;
				b[fy+6][8]=COLOR_WHITE;
				b[fy+7][1]=COLOR_WHITE;
				b[fy+7][9]=COLOR_WHITE;
			}
			HDC windowdc = GetDC(window);
			BitBlt(windowdc, 0, 0, b.sizeX(), b.sizeY(), dc[currentBuffer], 0, 0, SRCCOPY);
			ReleaseDC(window, windowdc);
#ifdef TBAL_EMULATE_DOUBLE_BUFFER
			currentBuffer = 1-currentBuffer;
#endif
		}
	};

	WinBitmap buffer;
	
	struct Timer 
	{
		bool isTimer;
		int interval;

		void Set (int interval) 
		{
			if (isTimer) Kill();
			if (interval>0 && window)
			{
				SetTimer(window, NULL, interval, 0);
				isTimer = true;
			} else 
				isTimer = false;
			this->interval = interval;
		}

		void Set () { Set (interval); }

		void Kill ()
		{
			if (isTimer) 
			{
				KillTimer(window, NULL);
				isTimer = false;
			}
		}

		Timer  () : isTimer(false), interval(-1) {}
		~Timer () { Kill(); } 
	};
	
	HWND GetWindowHandle()
	{
		return window;
	}

	Timer timer;
	bool active;

	bool usedLog = false;

	void LogI(const char* c)
	{
		std::ofstream f;
		f.open("log.txt", std::ios::app);
		if (!usedLog)
			f << std::endl << std::endl << "Beginning of log" << std::endl;
		f << "INFO " << c << std::endl;
		usedLog = true;
	}
	
	void LogW(const char* c)
	{
		std::ofstream f;
		f.open("log.txt", std::ios::app);
		if (!usedLog)
			f << std::endl << std::endl << "Beginning of log" << std::endl;
		f << "WARN " << c << std::endl;
		usedLog = true;
	}

	bool IsDirectoryExists(const char *filename)
	{
		DWORD dwFileAttributes = GetFileAttributes(filename);
		if (dwFileAttributes == 0xFFFFFFFF)
			return false;
		return (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}

	const char *GetExternalFilesDir ()
	{
		assert (started);
		const char* dir = "data/";

		if (!IsDirectoryExists(dir))
			CreateDirectory(dir, NULL);

		return dir;
	}
	
	void RunUrl (const char* url)
	{
		(void)url;
	}

	float GetXDPI ()
	{
		return 100.0;
	}

	float GetYDPI ()
	{
		return 100.0;
	}
	
	void SetTimer (int interval)
	{
		timer.Set(interval);
		tbal::interval = interval;
	};	

	bool windowMode = false;

	int bufSizeX, bufSizeY, newBufSizeX, newBufSizeY;

	bool ChangeRes(bool fullScreen, bool test)
	{
		DWORD dwFlags = test ? CDS_TEST : CDS_FULLSCREEN;

		if (fullScreen)
		{
			DEVMODE dm;
			dm.dmSize       = sizeof(dm);
			dm.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
			dm.dmPelsWidth  = bufSizeX; 
			dm.dmPelsHeight = bufSizeY;
			dm.dmBitsPerPel = 32;
			return (ChangeDisplaySettings(&dm , dwFlags) == DISP_CHANGE_SUCCESSFUL);
		} else
			return (ChangeDisplaySettings(NULL, dwFlags) == DISP_CHANGE_SUCCESSFUL);
	}

	LRESULT WINAPI WndProc (HWND h, UINT m, WPARAM w, LPARAM l)
	{
		switch (m) 
		{
		case WM_ACTIVATE: 
			if (!windowMode) ChangeRes(LOWORD(w) != WA_INACTIVE, false);
			if (LOWORD(w) == WA_INACTIVE)
			{
				timer.Kill();
				active = false;						
				if (!TbalMain(ACTION_MOVE, CODE_LOSTFOCUS, -1, -1))
					SendMessage (h, WM_CLOSE, 0, 0);
				#if TBAL_MOUSE_OVER
					ClipPointer(false);
				#endif
			} else
			{					
				timer.Set();
				active = true;
				#if TBAL_MOUSE_OVER
					ClipPointer(true);
				#endif
			}
			break;
		case WM_LBUTTONDOWN : if (enabled_mouse && !PointerDown(LOWORD(l),HIWORD(l))) SendMessage (h, WM_CLOSE, 0, 0); break;
		case WM_LBUTTONUP   : if (enabled_mouse && !PointerUp  (LOWORD(l),HIWORD(l))) SendMessage (h, WM_CLOSE, 0, 0); break;
		case WM_MOUSEMOVE   : if (enabled_mouse && !PointerMove(LOWORD(l),HIWORD(l))) SendMessage (h, WM_CLOSE, 0, 0); break;
		case WM_TIMER       : if (!SendTimerMessage())               SendMessage (h, WM_CLOSE, 0, 0); break;
		case WM_KEYDOWN     : if (!KeyDown(static_cast<Code>(w)))     SendMessage (h, WM_CLOSE, 0, 0); break;
		case WM_KEYUP       : if (!KeyUp  (static_cast<Code>(w)))     SendMessage (h, WM_CLOSE, 0, 0); break;
		case WM_PAINT       : buffer.SwapBuffers (); break;
		case WM_DESTROY     : PostQuitMessage(0); return 0;
		case WM_CLOSE       : TbalMain(ACTION_DOWN, CODE_EXIT, -1, -1); break;		
#ifndef TBAL_FIXED_SIZE
		case WM_SYSCOMMAND	: 
			if (w==SC_MAXIMIZE || w==SC_MINIMIZE)
			{				
				RECT cr, wr;
				GetWindowRect(h, &wr);
				GetClientRect(h, &cr);
			  newBufSizeX = cr.bottom-cr.top;
				newBufSizeY = cr.right-cr.left;
				if (!locked)
				{
					bufSizeX=newBufSizeX;
					bufSizeY=newBufSizeY;
					buffer.Init(bufSizeX,bufSizeY,sizeof(Color)*8);
				}
				const int sizex = newBufSizeX+(wr.right-wr.left)-(cr.right-cr.left);
				const int sizey = newBufSizeY+(wr.bottom-wr.top)-(cr.bottom-cr.top);
				MoveWindow(h, (wr.left+wr.right-sizex)/2, (wr.top+wr.bottom-sizey)/2, sizex, sizey, true);	
				return 0;
			}
			break;
#endif
		}
		return DefWindowProc (h,m,w,l);
	}

	void InitApplication(HINSTANCE hInstance, LPTSTR lpCmdLine)
	{
		// инициализируем оконный класс
		WNDCLASS wc;
		wc.style         = 0;
		wc.lpfnWndProc   = WndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = hInstance;
		wc.hIcon         = 0;
		wc.hCursor       = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = projectName;	    
		RegisterClass(&wc);

		// получаем размеры экрана
		HDC dc = GetDC(NULL);		
		int sx = GetDeviceCaps(dc, HORZRES);
		int sy = GetDeviceCaps(dc, VERTRES);
		ReleaseDC(NULL, dc);

		// ололопарсим командную строку
		std::vector<std::string> v;
		bool prevSpace = true;
		const char* cl = hardCmdLine ? hardCmdLine : lpCmdLine;
		for (const char*c=cl; *c; c++)
		{
			if (*c>' ')
			{
				if (prevSpace)
				{
					std::string last;
					v.push_back(last);
				} 
				v.back() += *c;
			}
			prevSpace = *c<=' ';
		}

		bufSizeX=sx;
		bufSizeY=sy;
		if (v.size()>=2)
		{
			int i;
			std::stringstream s0(v[0]);
			if (s0>>i) bufSizeX=i;
			std::stringstream s1(v[1]);
			if (s1>>i) bufSizeY=i;

			if (v.size()>=3 && v[2] == "w")	windowMode = true;
		} else
			windowMode=false;
		#if TBAL_VERTICAL
			bufSizeX = std::max(bufSizeY/2, std::min(bufSizeY*4/5, bufSizeX));
		#else
			bufSizeX = std::max(bufSizeY*5/4, std::min(bufSizeY*2, bufSizeX));
		#endif
	
		windowMode = windowMode || !ChangeRes(true, true);

		#ifndef TBAL_ALLOW_BIG_SIZE
			if (windowMode)
			{		
				bufSizeX = std::max(320, std::min(sx-20, bufSizeX));
				bufSizeY = std::max(200, std::min(sy-80, bufSizeY));
			}
		#endif

		if (windowMode)
		{
			#ifdef TBAL_FIXED_SIZE
				const DWORD style = WS_SYSMENU | WS_MINIMIZEBOX;
			#else
				const DWORD style = WS_SYSMENU | WS_MINIMIZEBOX	| WS_MAXIMIZEBOX;
			#endif

			window=CreateWindow(projectName, projectName, style,
				(sx-bufSizeX)/2, (sy-bufSizeY)/2, bufSizeX, bufSizeY, 0, 0, hInstance, 0);

			// выровнять размер
			RECT r;
			GetClientRect(window, &r);
			int wsx = bufSizeX*2-(r.right-r.left);
			int wsy = bufSizeY*2-(r.bottom-r.top);
			MoveWindow(window, (sx-wsx)/2, (sy-wsy)/2, wsx, wsy, false);
		} else
		{
			window=CreateWindow(projectName, projectName, WS_POPUP, 
				0, 0, bufSizeX, bufSizeY, 0, 0, hInstance, 0);
		}

		// всякая байда
		buffer.Init(bufSizeX, bufSizeY, sizeof(Color)*8);
		for (int i=0; i<0x100; i++) pressed[i] = false;

		newBufSizeX=bufSizeX;
		newBufSizeY=bufSizeY;
	}

	int RunApplication(int nCmdShow)
	{
    // вывести окно
		ShowWindow(window, nCmdShow);
		UpdateWindow(window);

		// главный цикл
		MSG m;

		#if TBAL_MOUSE_OVER
			ClipPointer(true);
		#endif
		#if TBAL_HIDE_CURSOR
			ShowCursor(false);
		#endif

		for(;;)
		{	
			if (interval || !active || PeekMessage(&m, NULL, 0, 0, 0))
			{
				if (!GetMessage(&m, NULL, 0, 0)) break;
				TranslateMessage(&m);
				DispatchMessage(&m);
			}
			if (interval==0 && active)
				if (!SendTimerMessage()) break;
		}
		#if TBAL_MOUSE_OVER
			ClipPointer(false);
		#endif

		return 0;
	}

	Buffer::Buffer() : Bitmap (buffer.buffers[buffer.currentBuffer])
	{
		assert(!locked);
		locked = true;
	}

	Buffer::~Buffer()
	{
		assert(locked);
		locked = false;
		buffer.SwapBuffers();

		if (bufSizeX != newBufSizeX || bufSizeY != newBufSizeY)
		{
			bufSizeX = newBufSizeX;
			bufSizeY = newBufSizeY;
			buffer.Init(bufSizeX, bufSizeY, sizeof(Color)*8);
		}
	}
		
	int MSec() 
	{
		SYSTEMTIME ft;
		GetSystemTime(&ft);
		return ft.wMilliseconds + 1000*(ft.wSecond + 60*(ft.wMinute+60*(ft.wHour)));	
	}

	Asset::Asset()
	{
		filename = "";
		loaded = false;
		buffer.resize(0);
	}

	void Asset::Open(tblib::stringref filename)
	{
		this->filename = std::string("assets/") + std::string(filename);
		loaded = false;
	}

	void Asset::Load()
	{
		if (!loaded)
		{
			std::ifstream f;
			f.open(filename.c_str(), std::ios::binary);
			if (f.is_open())
			{
				f.seekg (0, std::ios::end);
				buffer.resize(f.tellg());
				f.seekg (0, std::ios::beg);
				if (buffer.size())
					f.read(&buffer[0], (int)buffer.size());
			} else
				buffer.resize(0);
			loaded = true;
		}
	}

	tblib::stringref Asset::GetBuffer()
	{
		Load();
		if (int(buffer.size())>0)		
			return tblib::stringref(&buffer[0], 0, int(buffer.size()));
		else
			return tblib::stringref(NULL, 0, 0);
	}

	void Asset::GetDescriptor(FileDescriptor& fd)
	{
		fd.open(filename.c_str(), std::ios::binary);
	}

	Asset::~Asset() {}

#endif
	
	
#ifdef _WINDOWS
	bool Picture::Load(const char* fileName)
	{
		Asset pa;
		pa.Open(fileName);
		tblib::stringref data = pa.GetBuffer();
		int i = data.low();
		
		tagBITMAPFILEHEADER fileHeader;
		tagBITMAPINFOHEADER infoHeader;

		if (data.high()-i < sizeof(fileHeader))
			return false;
		memcpy(&fileHeader, &data[i], sizeof(fileHeader));
		if (int(fileHeader.bfSize) > data.high()-data.low())
			return false;
		i += sizeof(fileHeader);
		
		if (data.high()-i < sizeof(infoHeader))
			return false;
		memcpy(&infoHeader, &data[i], sizeof(infoHeader));

		bool b24=false;
		if (infoHeader.biBitCount==24 
		&& infoHeader.biBitCount+8 == sizeof(Color)*8) // мудацкое сравнение из-за бесполезного предупреждения
			b24=true;
		else if (infoHeader.biBitCount != sizeof(Color)*8)
			return false;

		i = fileHeader.bfOffBits;
		if (int(infoHeader.biWidth*infoHeader.biHeight*(infoHeader.biBitCount/8)) > data.high()-i)
			return false;

		tblib::recreate(*this, infoHeader.biWidth, infoHeader.biHeight);
		
		for (int j=m_sizeY-1; j>=0; --j)
		{
			if (b24)
			{
				tbal::Bitmap::line l=(*this)[j];
				int oi=i;
				for (int li=0; li<m_sizeX; ++li)
				{
					l[li] = tbal::Color(uint8_t(data[i+2]), uint8_t(data[i+1]), uint8_t(data[i+0]));
					i+=3;
				}
				i = oi+((i-oi+3)&(~3));
			}	else
			{
				memcpy(&(*this)[j][0], &data[i], m_sizeX*sizeof(Color));
				i += m_sizeX*sizeof(Color);
			}

		}

		return true;
	}

	bool Bitmap::Save (const char* fileName) const
	{

		tagBITMAPFILEHEADER fileHeader;
		tagBITMAPINFOHEADER infoHeader;

		fileHeader.bfType = 0x4D42;
		fileHeader.bfSize = sizeof(fileHeader) + sizeof(infoHeader) + m_sizeX*m_sizeY*sizeof(Color);
		fileHeader.bfReserved1 = 0;
		fileHeader.bfReserved2 = 0;
		fileHeader.bfOffBits = sizeof(fileHeader) + sizeof(infoHeader);

		infoHeader.biSize = sizeof(infoHeader);
		infoHeader.biBitCount = sizeof(Color)*8;
		infoHeader.biWidth = m_sizeX;
		infoHeader.biHeight = m_sizeY;
		infoHeader.biPlanes = 1;
		infoHeader.biCompression = BI_RGB;
		infoHeader.biSizeImage = 0;
	  infoHeader.biXPelsPerMeter = 3000;
	  infoHeader.biYPelsPerMeter = 3000;
		infoHeader.biClrUsed = sizeof(Color)==2 ? 65536 : 0;
		infoHeader.biClrImportant = 0;

		std::ofstream f;
		f.open((std::string(GetExternalFilesDir())+fileName).c_str(), std::ios::binary);
		if (f.is_open())
		{
			f.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
			f.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));
		
			for (int j=m_sizeY-1; j>=0; --j)
				f.write(reinterpret_cast<const char*>(&(*this)[j][0]), m_sizeX*sizeof(Color));

			f.flush();
			return true;
		} else
			return false;
	}
#endif



};

#ifdef _WINDOWS
	int APIENTRY WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
	{
		(void)hPrevInstance;
		TbalMain (tbal::ACTION_MOVE, tbal::CODE_GETNAME, 0, 0);
		tbal::InitApplication(hInstance, lpCmdLine);
		tbal::started = true;
		TbalMain (tbal::ACTION_MOVE, tbal::CODE_START, 0, 0);
		return tbal::RunApplication(nCmdShow);
	}
#endif
	
#ifdef ANDROID

	void android_main (android_app* state)
	{
		TbalMain (tbal::ACTION_MOVE, tbal::CODE_GETNAME, 0, 0);
		tbal::InitApplication(state);
		tbal::started = true;
		TbalMain (tbal::ACTION_MOVE, tbal::CODE_START, 0, 0);
		tbal::RunApplication(state);
	}
#endif
