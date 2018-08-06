class CalendarController < ApplicationController
  def show
    @date = params[:date] ? Date.parse(params[:date]) : Date.today

    @trainer = Trainer.find(params["format"])


  end
end















