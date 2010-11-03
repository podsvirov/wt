/*
 * Copyright (C) 2010 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

/*****
 * This file is part of the Wt::Dbo tutorial:
 * http://www.webtoolkit.eu/wt/doc/tutorial/dbo/tutorial.html
 *****/

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Sqlite3>
#include <string>

namespace dbo = Wt::Dbo;

/*****
 * Dbo tutorial section 6. Mapping relations
 *****/

class Post;
class User;
class Tag;

class Post {
public:
  dbo::ptr<User> user;
  dbo::collection< dbo::ptr<Tag> > tags;

  template<class Action>
  void persist(Action& a)
  {
    dbo::belongsTo(a, user, "user");
    dbo::hasMany(a, tags, dbo::ManyToMany, "post_tags");
  }
};

class Tag {
public:
  std::string name;
  dbo::collection< dbo::ptr<Post> > posts;

  template<class Action>
  void persist(Action& a)
  {
    dbo::field(a, name, "name");
    dbo::hasMany(a, posts, dbo::ManyToMany, "post_tags");
  }
};

class User {
public:
  enum Role {
    Visitor = 0,
    Admin = 1,
    Alien = 42
  };

  std::string name;
  std::string password;
  Role        role;
  int         karma;

  dbo::collection< dbo::ptr<Post> > posts;

  template<class Action>
  void persist(Action& a)
  {
    dbo::field(a, name,     "name");
    dbo::field(a, password, "password");
    dbo::field(a, role,     "role");
    dbo::field(a, karma,    "karma");

    dbo::hasMany(a, posts, dbo::ManyToOne, "user");
  }
};

void run()
{
  /*
   * Setup a session, would typically be done once at application startup.
   */
  dbo::backend::Sqlite3 sqlite3(":memory:");
  sqlite3.setProperty("show-queries", "true");
  dbo::Session session;
  session.setConnection(sqlite3);

  session.mapClass<User>("user");
  session.mapClass<Post>("post");
  session.mapClass<Tag>("tag");

  /*
   * Try to create the schema (will fail if already exists).
   */
  session.createTables();

  {
    dbo::Transaction transaction(session);

    User *user = new User();
    user->name = "Joe";
    user->password = "Secret";
    user->role = User::Visitor;
    user->karma = 13;

    dbo::ptr<User> userPtr = session.add(user);

    transaction.commit();
  }

  dbo::ptr<Post> post;
  {
    dbo::Transaction transaction(session);

    dbo::ptr<User> joe = session.find<User>().where("name = ?").bind("Joe");

    post = session.add(new Post());
    post.modify()->user = joe;

    // will print 'Joe has 1 post(s).'
    std::cerr << "Joe has " << joe->posts.size() << " post(s)." << std::endl;

    transaction.commit();
  }

  {
    dbo::Transaction transaction(session);

    dbo::ptr<Tag> cooking = session.add(new Tag());
    cooking.modify()->name = "Cooking";

    post.modify()->tags.insert(cooking);

    // will print '1 post(s) tagged with Cooking.'
    std::cerr << cooking->posts.size() << " post(s) tagged with Cooking." << std::endl;

    transaction.commit();
  }

}

int main(int argc, char **argv)
{
  run();
}