#include "../lib/hw.h"
#include "../h/MemoryAllocator.h"

    void* MemoryAllocator::kmemalloc(size_t size) {
        //asm volatile("csrr t6, sepc");

        memvals* p = nullptr;
        memvals* q = kfree;
        // u listi se trazi fragment dovoljne velicine po algoritmu first-fit.
        while (q and q->size < size) {
            p = q; q = q->next;
        }
        // ukoliko nema fragmenta koji je fit, a uvijek se radi polukompakcija, znaci da nema gdje da se stavi - vracamo nullptr.
        if (!q) return nullptr;
        // ako je tacno jednake velicine - prelancavanje
        // ovo se najvjerovatnije nikad i ne desi u mom kodu.
        if (q->size == size) {
            // kad se oduzme sve, moze se desiti da kfree postane null.
            if (p) p->next = q->next;
            else kfree = q->next;

            q->next = kbusy;
            kbusy = q;

            return q+1;

        }

        q->size -= size;

        p = (memvals*) ((char*)(q) + q->size*MEM_BLOCK_SIZE);
        /*
        char * t = (char*)p;
        for (uint64 i = 0 ; i < size*MEM_BLOCK_SIZE; i++) {
            *t = 0;
            t++;
        }
*/
        p->size = size;

        p->next = kbusy;
        kbusy = p;

        return p+1;
    }

    int MemoryAllocator::kmemfree (void * addr) {
        //asm volatile("csrr t6, sepc");
        memvals* p = nullptr;
        memvals* q = kbusy;
        // pretrazivanje liste zauzetih fragmenata
        while (q) {
            // ako je q+1 == addr, odnosno ako je to sto je u listi + 1 ono sto sam vratio kao addr, a sto se sad oslobadja, onda ce biti pronadjen i pretraga se zavrsava ranije.
            if (q+1 == addr) break;
            p = q; q = q->next;
        }
        // ako q nije pronadjen, vracamo -1 -> pokusavas da oslobodis memoriju koja nije ni zauzeta.
        if (!q) return -1;

        // inace, moramo da radimo prelancavanje.

        if (!p) kbusy = q->next;
        else p->next = q->next;

        q->next = nullptr;


        // prelazak u listu slobodnih.
        p = kfree;
        memvals *r = nullptr;

        // dok ne pronadjemo tacno mjesto za q - takvo da je p vece, a r manje od q, trazimo.
        while (p and p < q) {
            r = p;
            p = p->next;
        }
        // PODESAVANJE PRETHODNIKA

        // ako je u pitanju prvi element, onda je q->next = kfree, a kfree = q.
        if (!r) {
            q->next = kfree;
            kfree = q;
        }
        // ako je kraj prethodnika tacno pocetak sljedbenika - onda ih je moguce spojiti.
        else if ((char*)r + r->size*MEM_BLOCK_SIZE == (char*)q) {
            r->size += q->size;  // samo povecam velicinu prethodnika.
            q = r; // dakle, q->next = r->next
        }

        else {
            q->next = p;
            r->next = q;
        }
        // KOMPAKCIJA SLJEDBENIKA
        // ako je kraj od q pocetak od sljedbenika
        // povecaj q, sljedbenik od q je sad sljedbenik od p, a p nema sljedbenika
        if (p and (char*)q + q->size*MEM_BLOCK_SIZE == (char*)p) {
            q->size += p->size;
            q->next = p->next;
            p->next = nullptr;
        }
        return 0;
    }

