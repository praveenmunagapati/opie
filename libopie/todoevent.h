
#ifndef todoevent_h
#define todoevent_h

#include <qstringlist.h>
#include <qdatetime.h>

class ToDoEvent {
    friend class ToDoDB;
 public:
    // priorities from Very low to very high
    enum Priority { VERYHIGH=1, HIGH, NORMAL, LOW, VERYLOW };
    /* Constructs a new ToDoEvent
       @param completed Is the TodoEvent completed
       @param priority What is the priority of this ToDoEvent
       @param category Which category does it belong( uid )
       @param description What is this ToDoEvent about
       @param hasDate Does this Event got a deadline
       @param date what is the deadline?
       @param uid what is the UUID of this Event
    **/
    ToDoEvent( bool completed = false, int priority = NORMAL, 
	       const QStringList &category = QStringList(), 
	       const QString &description = QString::null , 
	       bool hasDate = false, QDate date = QDate::currentDate(), int uid = -1 );
    /* Copy c'tor

    **/
    ToDoEvent(const ToDoEvent & );

    /*
       Is this event completed?
    **/
    bool isCompleted() const;

    /*
       Does this Event have a deadline
    **/
    bool hasDate() const;

    /*
      What is the priority?
    **/
    int priority()const ;
    QStringList allCategories()const;
    QArray<int> categories() const;
    QDate date()const;
    QString description()const;

    QString richText() const;

    int uid()const { return m_uid;};
    void setCompleted(bool completed );
    void setHasDate( bool hasDate );
    // if the category doesn't exist we will create it
    // this sets the the Category after this call category will be the only category 
    void setCategory( const QString &category );
    // adds a category to the Categories of this event
    void insertCategory(const QString &category );
    void clearCategories();
    void setCategories(const QStringList& );

    void setPriority(int priority );
    void setDate( QDate date );
    void setDescription(const QString& );
    bool isOverdue();

    bool match( const QRegExp &r )const;

    void setUid(int id) {m_uid = id; };
    bool operator<(const ToDoEvent &toDoEvent )const;
    bool operator<=(const ToDoEvent &toDoEvent )const;
    bool operator!=(const ToDoEvent &toDoEvent )const { return !(*this == toDoEvent); };
    bool operator>(const ToDoEvent &toDoEvent )const;
    bool operator>=(const ToDoEvent &toDoEvent)const;
    bool operator==(const ToDoEvent &toDoEvent )const;
    ToDoEvent &operator=(const ToDoEvent &toDoEvent );
 private:
    class ToDoEventPrivate;
    ToDoEventPrivate *d;
    QDate m_date;
    bool m_isCompleted:1;
    bool m_hasDate:1;
    int m_priority;
    QStringList m_category;
    QString m_desc;
    int m_uid;
};


#endif
