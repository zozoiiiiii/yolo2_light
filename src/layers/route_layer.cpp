#include "route_layer.h"

NS_JJ_BEGIN

layer make_route_layer(int batch, int n, std::vector<int> input_layers, std::vector<int> input_sizes)
{
    fprintf(stderr, "route ");
    layer l;// = { 0 };
    l.type = ROUTE;
    l.batch = batch;
    l.n = n;
    l.input_layers = input_layers;
    l.input_sizes = input_sizes;
    int i;
    int outputs = 0;
    for (i = 0; i < n; ++i) {
        fprintf(stderr, " %d", input_layers[i]);
        outputs += input_sizes[i];
    }
    fprintf(stderr, "\n");
    l.outputs = outputs;
    l.inputs = outputs;
    l.output.assign(outputs*batch, 0.0f);
    l.output_int8.assign(outputs*batch, 0);
    return l;
}

bool RouteLayer::load(const IniParser* pParser, int section, size_params params)
{
    std::string l = pParser->ReadString(section, "layers");
    std::vector<int> results;
    StringUtil::splitInt(results, l, ",");
    int nSize = results.size();
    std::vector<int> layers;
    layers.assign(nSize, 0);
    std::vector<int> sizes;
    sizes.assign(nSize, 0);

    for (int i = 0; i < nSize; i++)
    {
        int index = results[i];
        if (index < 0)
            index = params.index + index;

        layers[i] = index;
        sizes[i] = params.net->jjLayers[index]->getLayer()->outputs;
    }

    int batch = params.batch;

    JJ::layer layer = make_route_layer(batch, nSize, layers, sizes);

    JJ::layer* first = params.net->jjLayers[layers[0]]->getLayer();
    layer.out_w = first->out_w;
    layer.out_h = first->out_h;
    layer.out_c = first->out_c;
    for (int i = 1; i < nSize; ++i)
    {
        int index = layers[i];
        JJ::layer* next = params.net->jjLayers[index]->getLayer();
        if (next->out_w == first->out_w && next->out_h == first->out_h)
        {
            layer.out_c += next->out_c;
        }
        else
        {
            layer.out_h = layer.out_w = layer.out_c = 0;
        }
    }
    //return layer;
    return true;
}



NS_JJ_END