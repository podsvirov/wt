#include "OidcUserInfoEndpoint"
#include <string>
#include <boost/algorithm/string.hpp>

#include "Wt/WResource"
#include "Wt/WObject"
#include "Wt/Http/Request"
#include "Wt/Http/Response"
#include "Wt/Auth/User"
#include "Wt/Auth/AbstractUserDatabase"
#include "Wt/Auth/IssuedToken"
#include "Wt/Http/Request"
#include "Wt/Http/Response"
#include "Wt/Json/Value"
#include "Wt/Json/Object"
#include "Wt/Json/Serializer"
#include "Wt/WLogger"

namespace {
const std::string AUTH_TYPE = "Bearer ";
}

namespace Wt {
namespace Auth {

LOGGER("OidcUserInfoEndpoint");

OidcUserInfoEndpoint::OidcUserInfoEndpoint(AbstractUserDatabase &db)
  : db_(&db)
{
  std::set<std::string> s1;
  s1.insert("name");
  setScopeToken("profile", s1);
  std::set<std::string> s2;
  s2.insert("email");
  s2.insert("email_verified");
  setScopeToken("email", s2);
}

void OidcUserInfoEndpoint::handleRequest(const Http::Request& request, Http::Response& response)
{
  std::string authHeader = request.headerValue("Authorization");
  if (!boost::starts_with(authHeader, AUTH_TYPE)) {
    response.setStatus(400);
    response.addHeader("WWW-Authenticate", "error=\"invalid_request\"");
    return;
  }
  std::string tokenValue = authHeader.substr(AUTH_TYPE.length());
  IssuedToken accessToken = db_->idpTokenFindWithValue("access_token", tokenValue);
  if (!accessToken.checkValid() || WDateTime::currentDateTime() > accessToken.expirationTime()) {
    response.setStatus(401);
    response.addHeader("WWW-Authenticate", "error=\"invalid_token\"");
    return;
  }
  response.setMimeType("application/json");
  response.setStatus(200);
  User user = accessToken.user();
  std::string scope = accessToken.scope();
  std::set<std::string> scopeSet;
  boost::split(scopeSet, scope, boost::is_any_of(" "));
#ifdef WT_TARGET_JAVA
  try {
#endif
    response.out() << Json::serialize(generateUserInfo(user, scopeSet)) << std::endl;
#ifdef WT_TARGET_JAVA
  } catch (std::io_exception ioe) {
    LOG_ERROR(ioe.message());
  }
#endif
}

Json::Object OidcUserInfoEndpoint::generateUserInfo(const User& user, const std::set<std::string>& scope)
{
  Json::Object root;
  root["sub"] = Json::Value(user.id());
  std::set<std::string> claims;
  for (std::set<std::string>::iterator s = scope.begin(); s != scope.end(); ++s) {
    const std::set<std::string>& c = claimMap_.find(*s)->second;
    for (std::set<std::string>::iterator s2 = c.begin(); s2 != c.end(); ++s2)
      claims.insert(*s2);
  }
  for (std::set<std::string>::iterator claim = claims.begin(); claim != claims.end(); ++claim) {
    Json::Value claimValue = db_->idpJsonClaim(user, *claim);
    if (!claimValue.isNull())
      root[*claim] = claimValue;
  }
  return root;
}

void OidcUserInfoEndpoint::setScopeToken(const std::string& scopeToken,
                                         const std::set<std::string>& claims)
{
  claimMap_[scopeToken] = claims;
}

const std::map<std::string,std::set<std::string> > &
OidcUserInfoEndpoint::scopeTokens() const
{
  return claimMap_;
}

}
}
