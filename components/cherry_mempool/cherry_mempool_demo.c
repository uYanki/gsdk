#include "gsdk.h"

#if CONFIG_DEMOS_SW

#include "chry_blockpool.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "mempool"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

#define BLOCK_SIZE      128
#define BLOCK_COUNT     16
#define POOL_SIZE       BLOCK_COUNT*(BLOCK_SIZE + sizeof(void*))

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

static chry_blockpool_t bp;

__ALIGNED(8)
static uint8_t mempool[POOL_SIZE];

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

void CherryMempool_Test(void)
{
    /**
     * The second parameter is the alignment size, given by the macro,
     * which serves to limit the alignment of each memory block,
     * the third parameter is the block size (bytes),
     * the fourth parameter is the memory pool,
     * which is preferably also aligned, the same as the second parameter,
     * the fifth parameter is the memory pool size,
     * which limits the maximum number of blocks this memory pool can be sliced into,
     * each block of the block memory pool also occupies additional
     * bytes of the pointer length, and Since the length of
     * the internal ringbuffer must be a power of 2,
     * there may be memory wastage due to alignment and ringbuffer length alignment,
     * and the total available blocks should be based on the number
     * obtained by chry_blockpool_get_size
     */

    if (0 == chry_blockpool_init(&bp, CHRY_BLOCKPOOL_ALIGN_8, BLOCK_SIZE, mempool, POOL_SIZE))
    {
        LOGD("success, total block [%d]", chry_blockpool_get_size(&bp));
    }
    else
    {
        LOGE("error");
    }
}

#endif
