from django.shortcuts import render
from django.views.generic import View
from django.http import JsonResponse
from rest_framework.views import APIView
from rest_framework.response import Response
from django.contrib.auth import get_user_model 

User = get_user_model()

def index(request, *args, **kwargs):
    return render(request, 'personal/charts.html', {})
def contact(request):
    return render(request, 'personal/basic.html',{'content':['If you would like to contact me, please email me.','hskinsley@gmail.com']})


def get_data(request, *args, **kwargs):
    data = {'sales':100,
            'customers': 10,}
    return JsonResponse(data)

class ChartData(APIView):

    authentication_classes = []
    permission_classes = []

    def get(self, request, format=None):
        count = User.objects.all().count()
        labels = ["User","Hello","dasas","Saad","daadasa"]
        defaultitems = [count,100,5,4,6]
        data = { 'labels': labels,   
                'defaultdata': defaultitems,}
        return Response(data)   
