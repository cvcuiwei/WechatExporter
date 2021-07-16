//
//  Exporter.h
//  WechatExporter
//
//  Created by Matthew on 2020/9/30.
//  Copyright © 2020 Matthew. All rights reserved.
//

#include <set>
#include <string>
#include <thread>
#include <atomic>

#include "Logger.h"
#include "PdfConverter.h"
#include "WechatObjects.h"
#include "ITunesParser.h"
#include "ExportNotifier.h"

// #define USING_ASYNC_TASK_FOR_MP3

#ifndef Exporter_h
#define Exporter_h

class MessageParser;
class TemplateValues;
class ExportContext;

class Exporter
{
protected:
    std::atomic_bool m_running;
    std::thread m_thread;

    // semaphore& m_signal;
    std::string m_workDir;
    
    WechatInfo m_wechatInfo;
    std::string m_backup;
    std::string m_output;
    Logger* m_logger;
    PdfConverter* m_pdfConverter;
    
    ITunesDb *m_iTunesDb;
    ITunesDb *m_iTunesDbShare;
    
    std::map<std::string, std::string> m_templates;
    std::map<std::string, std::string> m_localeStrings;

    ExportNotifier* m_notifier;
    
    std::atomic<bool> m_cancelled;
    int m_options;
    bool m_loadingDataOnScroll;
    std::string m_extName;
    std::string m_templatesName;
    
    std::map<std::string, std::map<std::string, void *>> m_usersAndSessionsFilter;
    
    std::vector<std::pair<Friend, std::vector<Session>>> m_usersAndSessions;
    
    ExportContext*  m_exportContext;

public:
    Exporter(const std::string& workDir, const std::string& backup, const std::string& output, Logger* logger, PdfConverter* pdfConverter);
    ~Exporter();

    void setNotifier(ExportNotifier *notifier);
    
    bool loadUsersAndSessions();
    void swapUsersAndSessions(std::vector<std::pair<Friend, std::vector<Session>>>& usersAndSessions);

    bool run();
    bool isRunning() const;
    void cancel();
    void waitForComplition();
    
    void filterUsersAndSessions(const std::map<std::string, std::map<std::string, void *>>& usersAndSessions);
    void setTextMode(bool textMode = true);
    void setPdfMode(bool pdfMode = true);
    void setOrder(bool asc = true);
    void saveFilesInSessionFolder(bool flags = true);
    void setSyncLoading(bool syncLoading = true);
    void setLoadingDataOnScroll(bool loadingDataOnScroll = true);
    void supportsFilter(bool supportsFilter = true);
    void setExtName(const std::string& extName);
    void setTemplatesName(const std::string& templatesName);
    
    std::string getITunesVersion() const;
    std::string getIOSVersion() const;
    std::string getWechatVersion() const;
    
    static void initializeExporter();
    static void uninitializeExporter();
    
    static bool hasPreviousExporting(const std::string& outputDir, int& options, std::string& exportTime);

protected:
    bool runImpl();
    bool exportUser(Friend& user, std::string& userOutputPath);
    // bool loadUserSessions(Friend& user, std::vector<Session>& sessions) const;
    bool loadUserFriendsAndSessions(const Friend& user, Friends& friends, std::vector<Session>& sessions, bool detailedInfo = true) const;
    int exportSession(const Friend& user, const MessageParser& msgParser, const Session& session, const std::string& userBase, const std::string& outputBase);
    
    bool exportMessage(const Session& session, const std::vector<TemplateValues>& tvs, std::vector<std::string>& messages);

    bool fillSession(Session& session, const Friends& friends) const;
    void releaseITunes();
    bool loadITunes(bool detailedInfo = true);
    bool loadTemplates();
    bool loadStrings();
    std::string getTemplate(const std::string& key) const;
    std::string getLocaleString(const std::string& key) const;
    
    void notifyStart();
    void notifyComplete(bool cancelled = false);
    void notifyProgress(uint32_t numberOfMessages, uint32_t numberOfTotalMessages);
    void notifySessionStart(const std::string& sessionUsrName, void * sessionData, uint32_t numberOfTotalMessages);
    void notifySessionComplete(const std::string& sessionUsrName, void * sessionData, bool cancelled = false);
    void notifySessionProgress(const std::string& sessionUsrName, void * sessionData, uint32_t numberOfMessages, uint32_t numberOfTotalMessages);
    void notifyTasksStart(const std::string& usrName, uint32_t numberOfTotalTasks);
    void notifyTasksComplete(const std::string& usrName, bool cancelled = false);
    void notifyTasksProgress(const std::string& usrName, uint32_t numberOfCompletedTasks, uint32_t numberOfTotalTasks);
    bool buildFileNameForUser(Friend& user, std::set<std::string>& existingFileNames);
    std::string buildContentFromTemplateValues(const TemplateValues& values) const;
    
    bool filterITunesFile(const char * file, int flags) const;
    
    static bool loadExportContext(const std::string& contextFile, ExportContext *context);
};

#endif /* Exporter_h */
