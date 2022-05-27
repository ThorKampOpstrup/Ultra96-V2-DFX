#include "SETUP.hpp"
#include "../lib/Partial_module.hpp"
#include "../lib/SD_card.hpp"
#include "../lib/Axi_gpio_controller.hpp"
#include "xil_types.h"
#include <stdlib.h>

Partial_module::Partial_module(SD_card *_card,
                               const char *_file_name,
                               XFpga *_XFpgaInstance_ref)
{
    // card = _card;
    XFpgaInstance_ref = _XFpgaInstance_ref;

    //!Note that the Heap size is increased to make malloc work
    bitstream_buffer_ptr = (unsigned char *)malloc(SIZE_OF_PARTIAL_BITSTREAM_BUFFER);
    //     Xil_Out32(XPAR_AXI_GPIO_4_BASEADDR, 1);
    status = _card->read_file(_file_name, bitstream_buffer_ptr, &bitFile_fileSize, SIZE_OF_PARTIAL_BITSTREAM_BUFFER);
    //     Xil_Out32(XPAR_AXI_GPIO_4_BASEADDR, 0);

    bitstream_buffer_ptr = (unsigned char *)realloc(bitstream_buffer_ptr, bitFile_fileSize);
    // unsigned char *test_buffer;
    // test_buffer = (unsigned char *)malloc(bitFile_fileSize);
    //  while (1)
    //  {
    //      memcpy(test_buffer, bitstream_buffer_ptr, bitFile_fileSize);
    //  }
    //  status = _card->write_file("test.bit", bitstream_buffer_ptr, bitFile_fileSize);
};

Partial_module::Partial_module(unsigned char *bitfile,
                               u32 size,
                               XFpga *_XFpgaInstance_ref)
{
    XFpgaInstance_ref = _XFpgaInstance_ref;
    bitstream_buffer_ptr = bitfile;
    bitFile_fileSize = size;
};

void Partial_module::set_rst_block(u8 _rst_pinN,
                                   Axi_gpio_controller *_axi_block)
{
    rst_pinN = _rst_pinN;
    axi_block = _axi_block;
};


//will only reset block if it has been linked
u32 Partial_module::reconfigure_PL(bool en_rst)
{
    if (en_rst && axi_block != NULL)
        axi_block->set_pin(rst_pinN);

    //! This 'XFpga_PL_BitStream_Load' will be deprecated in the 2022.1 release. Use the updated ' XFpga_BitStream_Load() ' API to perform the same functionality.
    status = XFpga_PL_BitStream_Load(XFpgaInstance_ref, (UINTPTR)bitstream_buffer_ptr, bitFile_fileSize, XFPGA_PARTIAL_EN);
    //status = XFpga_BitStream_Load(XFpgaInstance_ref, (UINTPTR)bitstream_buffer_ptr, (UINTPTR)key_addr, bitFile_fileSize, XFPGA_PARTIAL_EN);

    if (en_rst && axi_block != NULL)
    {
        usleep(RST_SLEEP_DURATION);
        axi_block->clear_pin(rst_pinN);
    }

    return status;
};

Partial_module::~Partial_module()
{
}
