/* Automatically generated by refactor.pl.
 *
 * CMDNAME: email
 * CMDLEVEL: QCMD_SECURE | QCMD_AUTHED
 * CMDARGS: 3
 * CMDDESC: Change your email address.
 * CMDFUNC: csa_doemail
 * CMDPROTO: int csa_doemail(void *source, int cargc, char **cargv);
 * CMDHELP: Usage: EMAIL <password> <email> <email>
 * CMDHELP: Changes your register email address.  Confirmation of the change will be sent
 * CMDHELP: both old and new addresses.  Where:
 * CMDHELP: password - your password
 * CMDHELP: email    - new email address.  Must be entered exactly the same way twice to avoid
 * CMDHELP:            mistakes.
 * CMDHELP: Note: due to the sensitive nature of this command, you must send the message 
 * CMDHELP: to Q@CServe.quakenet.org when using it.
 */

#include "../chanserv.h"
#include "../authlib.h"
#include "../../lib/irc_string.h"
#include <stdio.h>
#include <string.h>

int csa_doemail(void *source, int cargc, char **cargv) {
  reguser *rup, *ruh;
  nick *sender=source;
  maildomain *mdp, *smdp;
  char *local;
  char *dupemail;
  int found = 0;
  time_t t = time(NULL);
  maillock *mlp;

  if (cargc<3) {
    chanservstdmessage(sender, QM_NOTENOUGHPARAMS, "email");
    return CMD_ERROR;
  }

  if (!(rup=getreguserfromnick(sender)))
    return CMD_ERROR;

  if (!checkpassword(rup, cargv[0])) {
    chanservstdmessage(sender, QM_AUTHFAIL);
    cs_log(sender,"EMAIL FAIL username %s bad password %s",rup->username,cargv[0]);
    return CMD_ERROR;
  }

  if (strcmp(cargv[1],cargv[2])) {
    chanservstdmessage(sender, QM_EMAILDONTMATCH);
    cs_log(sender,"EMAIL FAIL username %s email don't match (%s vs %s)",rup->username,cargv[1],cargv[2]);
    return CMD_ERROR;
  }

  if(!UHasHelperPriv(rup) && (rup->lockuntil && rup->lockuntil > t)) {
    chanservstdmessage(sender, QM_ACCOUNTLOCKED, rup->lockuntil);
    return CMD_ERROR;
  }

  if(rup->email && !strcasecmp(cargv[1], rup->email->content)) {
    chanservstdmessage(sender, QM_EMAILMATCHESOLD);
    return CMD_ERROR;
  }

  if (csa_checkeboy(sender, cargv[1]))
    return CMD_ERROR;

  for(mlp=maillocks;mlp;mlp=mlp->next) {
    if(!match(mlp->pattern->content, cargv[1])) {
      chanservstdmessage(sender, QM_MAILLOCKED);
      return CMD_ERROR;
    }
  }

  dupemail = strdup(cargv[1]);
  local=strchr(dupemail, '@');
  if(!local)
    return CMD_ERROR;
  *(local++)='\0';

  mdp=findnearestmaildomain(local);
  if(mdp) {
    for(smdp=mdp; smdp; smdp=smdp->parent) {
      if(MDIsBanned(smdp)) {
        free(dupemail);
        chanservstdmessage(sender, QM_MAILLOCKED);
        return CMD_ERROR;
      }
      if((smdp->count >= smdp->limit) && (smdp->limit > 0)) {
        free(dupemail);
        chanservstdmessage(sender, QM_DOMAINLIMIT);
        return CMD_ERROR;
      }
    }
  }

  mdp=findmaildomainbydomain(local);
  if(mdp) {
    for (ruh=mdp->users; ruh; ruh=ruh->nextbydomain) {
      if (ruh->localpart)
        if (!strcasecmp(dupemail, ruh->localpart->content)) {
          found++;
        }
    }

    if((found >= mdp->actlimit) && (mdp->actlimit > 0)) {
      free(dupemail);
      chanservstdmessage(sender, QM_ADDRESSLIMIT);
      return CMD_ERROR;
    }
  }

  mdp=findorcreatemaildomain(cargv[1]);

  csdb_createmail(rup, QMAIL_NEWEMAIL);
  csdb_accounthistory_insert(sender, NULL, NULL, rup->email, getsstring(cargv[1], EMAILLEN));
  delreguserfrommaildomain(rup,rup->domain);

  if(rup->lastemail)
    freesstring(rup->lastemail);
  rup->lastemail=rup->email;
  rup->email=getsstring(cargv[1],EMAILLEN);
  rup->lastemailchange=t;
  rup->domain=findorcreatemaildomain(rup->email->content);
  if(!UHasHelperPriv(rup))
    rup->lockuntil=t+7*24*3600;
  addregusertomaildomain(rup, rup->domain);

  if(local) {
    rup->localpart=getsstring(dupemail,EMAILLEN);
  } else {
    rup->localpart=NULL;
  }
  free(dupemail);

  chanservstdmessage(sender, QM_EMAILCHANGED, cargv[1]);
  cs_log(sender,"EMAIL OK username %s",rup->username);

#ifdef AUTHGATE_WARNINGS
  if(UHasOperPriv(rup))
    chanservsendmessage(sender, "WARNING FOR PRIVILEGED USERS: you MUST go to https://auth.quakenet.org and login successfully to update the cache.");
#endif

  csdb_updateuser(rup);

  return CMD_OK;
}
