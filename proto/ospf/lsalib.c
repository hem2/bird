/*
 *	BIRD -- OSPF
 *
 *	(c) 1999-2000 Ondrej Filip <feela@network.cz>
 *
 *	Can be freely distributed and used under the terms of the GNU GPL.
 */

#include "ospf.h"

void
htonlsah(struct ospf_lsa_header *h, struct ospf_lsa_header *n)
{
  n->age=htons(h->age);
  n->options=h->options;
  n->type=h->type;
  n->id=htonl(h->id);
  n->rt=htonl(h->rt);
  n->sn=htonl(h->sn);
  n->checksum=htons(h->checksum);
  n->length=htons(h->length);
};

void
ntohlsah(struct ospf_lsa_header *n, struct ospf_lsa_header *h)
{
  h->age=ntohs(n->age);
  h->options=n->options;
  h->type=n->type;
  h->id=ntohl(n->id);
  h->rt=ntohl(n->rt);
  h->sn=ntohl(n->sn);
  h->checksum=ntohs(n->checksum);
  h->length=ntohs(n->length);
};

void
htonlsab(void *h, void *n, u8 type, u16 len)
{
  unsigned int i;
  switch(type)
  {
    case LSA_T_RT:
    {
      struct ospf_lsa_rt *hrt, *nrt;
      struct ospf_lsa_rt_link *hrtl,*nrtl;
      u16 links;

      nrt=n;
      hrt=h;
      links=hrt->links;

      nrt->VEB=hrt->VEB;
      nrt->padding=0;
      nrt->links=htons(hrt->links);
      nrtl=(struct ospf_lsa_rt_link *)(nrt+1);
      hrtl=(struct ospf_lsa_rt_link *)(hrt+1);
      for(i=0;i<links;i++)
      {
        (nrtl+i)->id=htonl((hrtl+i)->id);
        (nrtl+i)->data=htonl((hrtl+i)->data);
        (nrtl+i)->type=(hrtl+i)->type;
        (nrtl+i)->notos=(hrtl+i)->notos;
        (nrtl+i)->metric=htons((hrtl+i)->metric);
      }
      break;
    }
    case LSA_T_NET:
    {
      u32 *hid,*nid;

      nid=n;
      hid=h;

      for(i=0;i<(len/sizeof(u32));i++)
      {
        *(nid+i)=htonl(*(hid+i));
      }
      break;
    }
    case LSA_T_SUM_NET:
    case LSA_T_SUM_RT:
    {
      struct ospf_lsa_summ *hs, *ns;
      struct ospf_lsa_summ_net *hn, *nn;

      hs=h;
      ns=n;

      ns->netmask=htonl(hs->netmask);

      hn=(struct ospf_lsa_summ_net *)(hs+1);
      nn=(struct ospf_lsa_summ_net *)(ns+1);

      for(i=0;i<((len-sizeof(struct ospf_lsa_summ))/
        sizeof(struct ospf_lsa_summ_net));i++)
      {
        (nn+i)->tos=(hn+i)->tos;
	(nn+i)->metric=htons((hn+i)->metric);
	(nn+i)->padding=0;
      }
      break;
    }
    case LSA_T_EXT:
    {
      struct ospf_lsa_ext *he, *ne;
      struct ospf_lsa_ext_tos *ht, *nt;

      he=h;
      ne=n;

      ne->netmask=htonl(he->netmask);

      ht=(struct ospf_lsa_ext_tos *)(he+1);
      nt=(struct ospf_lsa_ext_tos *)(ne+1);

      for(i=0;i<((len-sizeof(struct ospf_lsa_ext))/
        sizeof(struct ospf_lsa_ext_tos));i++)
      {
        (nt+i)->etos=(ht+i)->etos;
        (nt+i)->padding=0;
        (nt+i)->metric=htons((ht+i)->metric);
        (nt+i)->fwaddr=htonl((ht+i)->fwaddr);
        (nt+i)->tag=htonl((ht+i)->tag);
      }
      break;
    }
    default: die("(hton): Unknown LSA\n");
  }
};

void
ntohlsab(void *n, void *h, u8 type, u16 len)
{
  unsigned int i;
  switch(type)
  {
    case LSA_T_RT:
    {
      struct ospf_lsa_rt *hrt, *nrt;
      struct ospf_lsa_rt_link *hrtl,*nrtl;
      u16 links;

      nrt=n;
      hrt=h;

      hrt->VEB=nrt->VEB;
      hrt->padding=0;
      links=hrt->links=ntohs(nrt->links);
      nrtl=(struct ospf_lsa_rt_link *)(nrt+1);
      hrtl=(struct ospf_lsa_rt_link *)(hrt+1);
      for(i=0;i<links;i++)
      {
        (hrtl+i)->id=ntohl((nrtl+i)->id);
        (hrtl+i)->data=ntohl((nrtl+i)->data);
        (hrtl+i)->type=(nrtl+i)->type;
        (hrtl+i)->notos=(nrtl+i)->notos;
        (hrtl+i)->metric=ntohs((nrtl+i)->metric);
      }
      break;
    }
    case LSA_T_NET:
    {
      u32 *hid,*nid;

      hid=h;
      nid=n;

      for(i=0;i<(len/sizeof(u32));i++)
      {
        *(hid+i)=ntohl(*(nid+i));
      }
      break;
    }
    case LSA_T_SUM_NET:
    case LSA_T_SUM_RT:
    {
      struct ospf_lsa_summ *hs, *ns;
      struct ospf_lsa_summ_net *hn, *nn;

      hs=h;
      ns=n;

      hs->netmask=ntohl(ns->netmask);

      hn=(struct ospf_lsa_summ_net *)(hs+1);
      nn=(struct ospf_lsa_summ_net *)(ns+1);

      for(i=0;i<((len-sizeof(struct ospf_lsa_summ))/
        sizeof(struct ospf_lsa_summ_net));i++)
      {
        (hn+i)->tos=(nn+i)->tos;
	(hn+i)->metric=ntohs((nn+i)->metric);
	(hn+i)->padding=0;
      }
      break;
    }
    case LSA_T_EXT:
    {
      struct ospf_lsa_ext *he, *ne;
      struct ospf_lsa_ext_tos *ht, *nt;

      he=h;
      ne=n;

      he->netmask=ntohl(ne->netmask);

      ht=(struct ospf_lsa_ext_tos *)(he+1);
      nt=(struct ospf_lsa_ext_tos *)(ne+1);

      for(i=0;i<((len-sizeof(struct ospf_lsa_ext))/
        sizeof(struct ospf_lsa_ext_tos));i++)
      {
        (ht+i)->etos=(nt+i)->etos;
        (ht+i)->padding=0;
        (ht+i)->metric=ntohs((nt+i)->metric);
        (ht+i)->fwaddr=ntohl((nt+i)->fwaddr);
        (ht+i)->tag=ntohl((nt+i)->tag);
      }
      break;
    }
    default: die("(ntoh): Unknown LSA\n");
  }
};

#define MODX 4102		/* larges signed value without overflow */

/* Fletcher Checksum -- Refer to RFC1008. */
#define MODX                 4102
#define LSA_CHECKSUM_OFFSET    15

/* FIXME This is VERY uneficient, I have huge endianity problems */
void
lsasum_calculate(struct ospf_lsa_header *h,void *body,struct proto_ospf *po)
{
  u16 length;
  
  length=h->length;

  htonlsah(h,h);
  htonlsab(body,body,h->type,length);

  (void)lsasum_check(h,body,po);
  
  ntohlsah(h,h);
  ntohlsab(body,body,h->type,length);
}

/*
 * Note, that this function expects that LSA is in big endianity
 * It also returns value in big endian
 */
u16
lsasum_check(struct ospf_lsa_header *h,void *body,struct proto_ospf *po)
{
  u8 *sp, *ep, *p, *q, *b;
  int c0 = 0, c1 = 0;
  int x, y;
  u16 length,chsum;

  b=body;
  sp = (char *) &h->options;
  length=ntohs(h->length)-2;
  h->checksum = 0;

  for (ep = sp + length; sp < ep; sp = q)
  {		/* Actually MODX is very large, do we need the for-cyclus? */
    q = sp + MODX;
    if (q > ep) q = ep;
    for (p = sp; p < q; p++)
    {
      /* 
       * I count with bytes from header and than from body
       * but if there is no body, it's appended to header
       * (probably checksum in update receiving) and I go on
       * after header
       */
      if((b==NULL) || (p<(u8 *)(h+1)))
      {
	      c0 += *p;
      }
      else
      {
	      c0 += *(b+(p-sp)-sizeof(struct ospf_lsa_header)+2);
      }

      c1 += c0;
    }
    c0 %= 255;
    c1 %= 255;
  }

  x = ((length - LSA_CHECKSUM_OFFSET) * c0 - c1) % 255;
  if (x <= 0) x += 255;
  y = 510 - c0 - x;
  if (y > 255) y -= 255;

  chsum= x + (y << 8);
  h->checksum = chsum;
  return chsum;
}

int
lsa_comp(struct ospf_lsa_header *l1, struct ospf_lsa_header *l2)
			/* Return codes from point of view of l1 */
{
  if(l1->sn<l2->sn) return CMP_NEWER;
    if(l1->sn==l2->sn)
    {
      if(l1->checksum=!l2->checksum)
        return l1->checksum<l2->checksum ? CMP_OLDER : CMP_NEWER;
      if(l1->age==LSA_MAXAGE) return CMP_NEWER;
      if(l2->age==LSA_MAXAGE) return CMP_OLDER;
      if(abs(l1->age-l2->age)>LSA_MAXAGEDIFF)
        return l1->age<l2->age ? CMP_NEWER : CMP_OLDER;
    }
    return CMP_SAME;
}

/* LSA can be temporarrily, but body must be mb_alloced. */
struct top_hash_entry *
lsa_install_new(struct ospf_lsa_header *lsa, void *body, struct ospf_area *oa)
{
  int change=0,i;
  struct top_hash_entry *en;

  if((en=ospf_hash_find_header(oa->gr,lsa))==NULL)
  {
    en=ospf_hash_get_header(oa->gr,lsa);
    change=1;
    s_add_tail(&oa->lsal, SNODE en);
  }
  else
  {
    if(en->lsa.options!=lsa->options) change=1;
    if((en->lsa.age==LSA_MAXAGE)||(lsa->age==LSA_MAXAGE)) change=1;
    if(en->lsa.length!=lsa->length) change=1;
    else
    {
      u8 *k=en->lsa_body,*l=body;
      for(i=0;i<lsa->length;i++)
      {
        if(*(k+i)!=*(l+i))
	{
	  change=1;
	  break;
	}
      }
      s_rem_node(SNODE en);
      s_add_tail(&oa->lsal, SNODE en);
    }
  }
  en->inst_t=now;
  if(en->lsa_body!=NULL)mb_free(en->lsa_body);
  en->lsa_body=body;
  memcpy(&en->lsa,lsa,sizeof(struct ospf_lsa_header));
  
  /* FIXME decide if route calcualtion must be done and how */
  return en;
}