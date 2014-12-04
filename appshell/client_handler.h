// Copyright (c) 2011 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFCLIENT_CLIENT_HANDLER_H_
#define CEF_TESTS_CEFCLIENT_CLIENT_HANDLER_H_
#pragma once

#include <map>
#include <set>
#include <string>
#include "include/base/cef_lock.h"
#include "include/cef_client.h"
#include "util.h"
#include "command_callbacks.h"

#include <algorithm> 

#if defined(OS_LINUX)
// The Linux client uses GTK instead of the underlying platform type (X11).
#include <gtk/gtk.h>
#define ClientWindowHandle GtkWidget*
#else
#define ClientWindowHandle CefWindowHandle
#endif

// Define this value to redirect all popup URLs to the main application browser
// window.
// #define TEST_REDIRECT_POPUP_URLS

// ClientHandler implementation.
class ClientHandler : public CefClient,
                      public CefLifeSpanHandler,
                      public CefDragHandler,
                      public CefLoadHandler,
                      public CefRequestHandler,
                      public CefDisplayHandler,
                      public CefKeyboardHandler,
                      public CefGeolocationHandler,
                      public CefContextMenuHandler {
 
public:
  // Interface for process message delegates. Do not perform work in the
  // RenderDelegate constructor.
  class ProcessMessageDelegate : public virtual CefBase {
   public:
    // Called when a process message is received. Return true if the message was
    // handled and should not be passed on to other handlers.
    // ProcessMessageDelegates should check for unique message names to avoid
    // interfering with each other.
    virtual bool OnProcessMessageReceived(
        CefRefPtr<ClientHandler> handler,
        CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) {
      return false;
    }
  };

  typedef std::set<CefRefPtr<ProcessMessageDelegate> >
      ProcessMessageDelegateSet;

  // Interface for request handler delegates. Do not perform work in the
  // RequestDelegate constructor.
  class RequestDelegate : public virtual CefBase {
   public:
    // Called to retrieve a resource handler.
    virtual CefRefPtr<CefResourceHandler> GetResourceHandler(
        CefRefPtr<ClientHandler> handler,
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request) {
      return NULL;
    }
  };

  typedef std::set<CefRefPtr<RequestDelegate> > RequestDelegateSet;

  ClientHandler();
  virtual ~ClientHandler();

  // CefClient methods
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefDragHandler> GetDragHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefGeolocationHandler> GetGeolocationHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE {
    return this;
  }
                          
  virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message)
                                        OVERRIDE;

  virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE {
    return this;
  } 

  // CefLifeSpanHandler methods
  virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             const CefString& target_url,
                             const CefString& target_frame_name,
                             const CefPopupFeatures& popupFeatures,
                             CefWindowInfo& windowInfo,
                             CefRefPtr<CefClient>& client,
                             CefBrowserSettings& settings,
                             bool* no_javascript_access) OVERRIDE;
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
  virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
  
  // CefDragHandler methods
  virtual bool OnDragEnter(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefDragData> dragData,
                           DragOperationsMask mask) OVERRIDE;

  // CefLoadHandler methods
  virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame) OVERRIDE;
  virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int httpStatusCode) OVERRIDE;
  virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           ErrorCode errorCode,
                           const CefString& errorText,
                           const CefString& failedUrl) OVERRIDE;

  // CefRequestHandler methods
  virtual CefRefPtr<CefResourceHandler> GetResourceHandler(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request) OVERRIDE;

  // CefDisplayHandler methods
  virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                    bool isLoading,
                                    bool canGoBack,
                                    bool canGoForward) OVERRIDE;
  virtual void OnAddressChange(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               const CefString& url) OVERRIDE;
  virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
                             const CefString& title) OVERRIDE;
  virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                const CefString& message,
                                const CefString& source,
                                int line) OVERRIDE;

  // CefGeolocationHandler methods
  virtual bool OnRequestGeolocationPermission(
      CefRefPtr<CefBrowser> browser,
      const CefString& requesting_url,
      int request_id,
      CefRefPtr<CefGeolocationCallback> callback) OVERRIDE;

  // CefContextMenuHandler methods
  virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefContextMenuParams> params,
                                   CefRefPtr<CefMenuModel> model) OVERRIDE;
  virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefContextMenuParams> params,
                                    int command_id,
                                    EventFlags event_flags) OVERRIDE;

  // CefKeyboardHandler methods
  virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                                    const CefKeyEvent& event,
                                    CefEventHandle os_event,
                                    bool* is_keyboard_shortcut) OVERRIDE;

  virtual bool OnKeyEvent(CefRefPtr<CefBrowser> browser,
                                    const CefKeyEvent& event,
                                    CefEventHandle os_event) OVERRIDE;

  void SetMainHwnd(ClientWindowHandle hwnd);
  ClientWindowHandle GetMainHwnd() { return m_MainHwnd; }
  void SetEditHwnd(ClientWindowHandle hwnd);
  void SetButtonHwnds(ClientWindowHandle backHwnd,
                      ClientWindowHandle forwardHwnd,
                      ClientWindowHandle reloadHwnd,
                      ClientWindowHandle stopHwnd);

  CefRefPtr<CefBrowser> GetBrowser() { return m_Browser; }
  int GetBrowserId() { return m_BrowserId; }
  bool CanCloseBrowser(CefRefPtr<CefBrowser> browser);

  std::string GetLogFile();

  void SetLastDownloadFile(const std::string& fileName);
  std::string GetLastDownloadFile();

  // Send a notification to the application. Notifications should not block the
  // caller.
  enum NotificationType {
    NOTIFY_CONSOLE_MESSAGE,
    NOTIFY_DOWNLOAD_COMPLETE,
    NOTIFY_DOWNLOAD_ERROR,
  };
  void SendNotification(NotificationType type);
  void CloseMainWindow();

  void ShowDevTools(CefRefPtr<CefBrowser> browser);  
                        
  // Call the "executeCommand" method, passing the command name.
  // If callback is specified, it will be called with the result from the command.
  bool SendJSCommand(CefRefPtr<CefBrowser> browser, const CefString& command, CefRefPtr<CommandCallback> callback = NULL);
  
  void SendOpenFileCommand(CefRefPtr<CefBrowser> browser, const CefString& filename);
  void DispatchCloseToNextBrowser();
  void AbortQuit();
  static CefRefPtr<CefBrowser> GetBrowserForNativeWindow(void* window);

  void QuittingApp(bool quitting) { m_quitting = quitting; }
  bool AppIsQuitting() { return m_quitting; }
  bool HasWindows() const { return !browser_window_map_.empty(); }

 protected:
  void SetLoading(bool isLoading);
  void SetNavState(bool canGoBack, bool canGoForward);
  void PopupCreated(CefRefPtr<CefBrowser> browser);
  void ComputePopupPlacement(CefWindowInfo& windowInfo);
                        
  // Create all of ProcessMessageDelegate objects.
  static void CreateProcessMessageDelegates(
      ProcessMessageDelegateSet& delegates);

  // Create all of RequestDelegateSet objects.
  static void CreateRequestDelegates(RequestDelegateSet& delegates);

  // The main frame window handle
  ClientWindowHandle m_MainHwnd;

  // The child browser window. This is only set for the FIRST client window.
  // The browser id for m_Browser is stored in m_BrowserId.
  //
  // On Windows, the FIRST client window is the main window that must be closed last.
  //
  // On Mac, the FIRST client window does not necessarily need to be closed last.
  // A lot of the code seems to unnecessarily enforce that only certain operations will
  // work if m_Browser/m_BrowserId is set. TODO: fix this.
  CefRefPtr<CefBrowser> m_Browser;

  // The child browser id of m_Browser. This is also only set for FIRST client window.
  // See comments above for m_Browser.
  int m_BrowserId;

  // The edit window handle
  ClientWindowHandle m_EditHwnd;

  // The button window handles
  ClientWindowHandle m_BackHwnd;
  ClientWindowHandle m_ForwardHwnd;
  ClientWindowHandle m_StopHwnd;
  ClientWindowHandle m_ReloadHwnd;
                          
  // Support for logging.
  std::string m_LogFile;

  // Support for downloading files.
  std::string m_LastDownloadFile;

  // True if a form element currently has focus
  bool m_bFormElementHasFocus;
  bool m_quitting;

  // Registered delegates.
  ProcessMessageDelegateSet process_message_delegates_;
  RequestDelegateSet request_delegates_;

  typedef std::map< ClientWindowHandle, CefRefPtr<CefBrowser> > BrowserWindowMap;
  static BrowserWindowMap browser_window_map_;
                        
  typedef std::map<int32, CefRefPtr<CommandCallback> > CommandCallbackMap;
  int32 callbackId;
  CommandCallbackMap command_callback_map_;

  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(ClientHandler);
  // Include the default locking implementation.
  IMPLEMENT_LOCKING(ClientHandler);
};

#endif  // CEF_TESTS_CEFCLIENT_CLIENT_HANDLER_H_
